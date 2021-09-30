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
  
// comando built-in de ajuda
void openHelp()
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
        openHelp();
        return 1;
    default:
        break;
    }
    return 0;
}
  
// function for parsing command words
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
  
int process_string(char* str, char** parsed)
{
    char* piped_string[2];
    int piped = 0;
  
    if (piped){
        parse_comms(piped_string[0], parsed);
  
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
        flag = process_string(input, args);
        // a flag retornará 0 se não houver comando ou se for built-in
        // retornará 1 se for um comando simples
        if (flag == 1)
            execute(args);
    }
    return 0;
}