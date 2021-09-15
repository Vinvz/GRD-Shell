#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define GRD_RL_BUFSIZE 1024
#define GRD_TOK_BUFSIZE 64
#define GRD_TOK_DELIM " \t\r\n\a"


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

int GRD_execute(char **args)
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

int main(int argc, char **argv)
{
    // carregar arquivos de config

    GRD_loop();

    // processos de shutdown e limpeza

    return EXIT_SUCCESS;
}