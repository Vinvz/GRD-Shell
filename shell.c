#include <stdio.h>
#include <stdin.h>
#include <stdlib.h>

void GRD_loop()
{
    char *line;
    char **args;
    int status;

    do{
        printf("GRD >> "); //o atual prompt do shell
        line = GRD_read_line();
        args = GRD_split_line();
        status = GRD_execute();

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