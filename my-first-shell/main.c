#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM "\t\r\n\a"
char **lsh_split_line(char *line)
{

}

#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

}

void lsh_loop(void)
{
    printf("lsh_loop\n");
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = lsh_read_line();
        // args = lsh_split_line(line);
        // status = lsh_execute(args);

        free(line);
        // free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    lsh_loop();
    printf("main\n");
    return EXIT_SUCCESS;
}
