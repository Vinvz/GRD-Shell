#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define GRD_RL_BUFSIZE 1024
#define GRD_TOK_BUFSIZE 64
#define GRD_TOK_DELIM " \t\r\n\a"

// declaração das funções built-in
int GRD_cd(char **args);
int GRD_help(char **args);
int GRD_exit(char **args);

// lista dos comandos built-in
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
};

int (*builtin_func[]) (char **) = {
    &GRD_cd,
    &GRD_help,
    &GRD_exit
};

int GRD_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
};

// funções built-in
int GRD_help(char **args)
{
    int i;
    printf("GRD Shell\n");
    printf("Esses são os comandos disponíveis deste shell:\n");

    for (i = 0; i < GRD_num_builtins(); i++){
        printf("  %s\n", builtin_str[i]);
    }

    return 1;
}

int GRD_cd(char **args)
{
    if (args[1] == NULL){
        fprintf(stderr, "GRD: argumento esperado para 'cd'\n");
    } else{
        if (chdir(args[1]) != 0){
            perror("GRD");
        }
    }
    return 1;
}


int GRD_exit(char **args)
{
    return 0;
}

char *GRD_read(void)
{
    int bufsize = GRD_RL_BUFSIZE;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (buffer == NULL){
        fprintf(stderr, "GRD: erro de alocação\n");
        exit(EXIT_FAILURE);
    }

    while (1){
        c = getchar();

        if (c == EOF || c == '\n'){
            buffer[pos] = '\0';
            return buffer;
        } else {
            buffer[pos] = c;
        }
        pos++;

        if (pos >= bufsize) {
            bufsize += GRD_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);

            if (buffer == NULL) {
                fprintf(stderr, "GRD: erro de alocação\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **GRD_parse(char *line)
{
    int bufsize = GRD_TOK_BUFSIZE, pos = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (tokens == NULL){
        fprintf(stderr, "GRD: Erro de alocação\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, GRD_TOK_DELIM);
    while (token != NULL) {
        tokens[pos] = token;
        pos++;

        if (pos >= bufsize) {
            bufsize += GRD_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (tokens == NULL){
                fprintf(stderr, "GRD: Erro de alocação\n");
                exit(EXIT_FAILURE);
            }
        }

        token == strtok(NULL, GRD_TOK_DELIM);
    }
    
    tokens[pos] = NULL;
    return tokens;
}

int GRD_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0){
        // processo filho
        if (execvp(args[0], args) == -1){
            perror("GRD");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0){
        // forking de erros
        perror("GRD");
    } else {
        // processo pai
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int GRD_execute(char **args)
{
    int i;

    if (args[0] == NULL){ // um comando vazio foi inserido
        return 1;
    }

    for (i = 0; i < GRD_num_builtins(); i++){
        if (strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i]) (args);
        }
    }

    return GRD_launch(args);
}

void GRD_loop()
{
    char *line;
    char **args;
    int status;

    do{
        printf("\nGRD >> "); //o atual prompt do shell
        line = GRD_read();
        args = GRD_parse(line);
        status = GRD_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    // carregar arquivos de config

    GRD_loop();

    // processos de shutdown e limpeza

    return EXIT_SUCCESS;
}