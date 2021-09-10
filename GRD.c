#include <stdio.h>
#include <stdlib.h>

#define GRD_RL_BUFSIZE 1024


void GRD_loop()
{
    char *line;
    char **args;
    int status;

    do{
        printf("\nGRD >> "); //o atual prompt do shell
        line = GRD_read_line();
        args = GRD_split_line(line);
        status = GRD_execute(args);

        free(line);
        free(args);
    } while (status);
}

char *GRD_read_line(void)
{
    int bufsize = GRD_RL_BUFSIZE;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (buffer == NULL){
        fprintf(stderr, "GRD: erro de alocacao\n");
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
                fprintf(stderr, "GRD: erro de alocacao");
                exit(EXIT_FAILURE);
            }
        }
    }
}



int main(int argc, char **argv)
{
    // carregar arquivos de config

    GRD_loop();

    // processos de shutdown e limpeza

    return EXIT_SUCCESS;
}