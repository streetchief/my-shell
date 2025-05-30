#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function declarations for builtin shell commands
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

// List of builtin commands and corresponding functions
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

// builtin implementations

// change directory
int lsh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: lsh_cd: expected argumetn to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }

    return 1;
}

// display help text
int lsh_help(char **args)
{
    int i;
    printf("Sam Leavin's LSH\n");
    printf("Type program name and arguments, and hit enter.\n");
    printf("The following builtins are supported:\n");

    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

// shell processing

int lsh_launch(char **args)
{
    pid_t pid;
    pid_t wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }

        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("lsh");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int lsh_execute(char **args)
{
    int i;

    if (args[0] == NULL) {
        // an empty command was entered
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM "\t\r\n\a"
char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: split_line: allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);

    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) {
                fprintf(stderr, "lsh: split_line: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;

}

#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int ch;

    if (!buffer) {
        fprintf(stderr, "lsh: lsh_read_line: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        ch = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (ch == EOF || ch == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = ch;
        }

        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);

            if (!buffer) {
                fprintf(stderr, "lsh: lsh_read_line: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
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
        args = lsh_split_line(line);
        status = lsh_execute(args);
        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    printf("main\n");
    // Load config files, if any.

    // Run command loop.
    lsh_loop();

    // Perform any shutdown/cleanup.
    printf("finished\n");
    return EXIT_SUCCESS;
}
