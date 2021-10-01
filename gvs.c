#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<readline/readline.h>
  
#define MAXLETTER 1000
#define MAXCOMM 100

int read_input(char* str)
{
    char* buffer;
    buffer = readline("\nGVS > ");
    
    if (strlen(buffer) != 0){
        strcpy(str, buffer);
        return 0;
    } else {
        return 1;
    }
}

void execute(char** parsed)
{
    pid_t pid = fork(); // criando o fork filho

    if (pid == -1) {
        printf("\nProcesso de fork falhou.");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0){
            printf("\nNão foi possível executar o comando");
        }
        exit(0);
    } else {
        wait(NULL); // esperando pelo fim do processo filho
        return;
    }
}

// função para executar pipes
void execute_pipe(char** parsed, char** parsedpipe)
{
    // 0 é final de leitura, 1 é final de escrita
    int pipefd[2]; 
    pid_t p1, p2;
  
    if (pipe(pipefd) < 0) {
        printf("\nPipe não foi inicializado");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nNão foi possível forkar");
        return;
    }
  
    if (p1 == 0) { // filho executando
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
  
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nNão foi possível executar o comando 1");
            exit(0);
        }
    } else { // pai executando
        p2 = fork();
  
        if (p2 < 0) {
            printf("\nNão foi possível forkar");
            return;
        }
  
        if (p2 == 0) { // filho 2 executando
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nNão foi possível executar o comando 2");
                exit(0);
            }
        } else { // pai executando, esperando pelos 2 filhos
            wait(NULL);
            wait(NULL);
        }
    }
}

// comando built-in de ajuda
void help()
{
    puts("Essa é a aba de ajuda do GVShell."
        "\nEsses são so comandos disponíveis:"
        "\n- cd: Troca o diretório atual"
        "\n- ls: Lista os arquivos no diretório"
        "\n- exit: Encerra o shell"
        "\n- Comandos gerais encontrados em Shells Unix"
        "\nTambém há suporte para o uso de pipes.");
    return;
}
  
// Função para executar comandos built-in
int builtin_handler(char** parsed)
{
    int num_commands = 4, i, on_args = 0;
    char* list_commands[num_commands];
    char* username;

    list_commands[0] = "exit";
    list_commands[1] = "cd";
    list_commands[2] = "help";
  
    for (i = 0; i < num_commands; i++){
        if (strcmp(parsed[0], list_commands[i]) == 0) {
            on_args = i + 1;
            break;
        }
    }
  
    switch (on_args){
    case 1:
        printf("\nEncerrando Shell\n");
        exit(0);
    case 2:
        chdir(parsed[1]);
        return 1;
    case 3:
        help();
        return 1;
    default:
        break;
    }
    return 0;
}

// função para achar pipes
int pipe_parser(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }
  
    if (strpiped[1] == NULL)
        return 0; // retorna zero caso pipe nulo
    else {
        return 1;
    }
}

// função para achar comandos simples
void parse_comms(char* str, char** parsed)
{
    int i;

    for (i = 0; i < MAXCOMM; i++){
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL){
            break;
        }
        if (strlen(parsed[i]) == 0){
            i--;
        }
    }
}
  
int process_string(char* str, char** parsed, char **parsedpipe)
{
    char* piped_string[2];
    int piped = 0;
  
    if (piped){
        parse_comms(piped_string[0], parsed);
        parse_comms(piped_string[1], parsedpipe);
  
    } else {
        parse_comms(str, parsed);
    }
  
    if (builtin_handler(parsed)){
        return 0;
    } else {
        return 1 + piped;
    }
}
  
int main(void)
{
    char input[MAXLETTER], *args[MAXCOMM];
    char* piped_args[MAXCOMM];
    int flag = 0;
  
    while (1){
        if (read_input(input))
            continue;
        flag = process_string(input, args, piped_args);
        // a flag retornará 0 se não houver comando ou se for built-in
        // retornará 1 se for um comando simples
        // retornará 2 se for um comando contendo pipes
        if (flag == 1)
            execute(args);
        if (flag == 2)
            execute_pipe(args, piped_args);
    }
    return 0;
}