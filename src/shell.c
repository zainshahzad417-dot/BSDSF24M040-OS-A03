#include "shell.h"

char* read_cmd(char* prompt, FILE* fp) {
    printf("%s", prompt);
    char* cmdline = (char*) malloc(sizeof(char) * MAX_LEN);
    int c, pos = 0;

    while ((c = getc(fp)) != EOF) {
        if (c == '\n') break;
        cmdline[pos++] = c;
    }

    if (c == EOF && pos == 0) {
        free(cmdline);
        return NULL; // Handle Ctrl+D
    }
    
    cmdline[pos] = '\0';
    return cmdline;
}

char** tokenize(char* cmdline) {
    // Edge case: empty command line
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n') {
        return NULL;
    }

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = (char*)malloc(sizeof(char) * ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    char* cp = cmdline;
    char* start;
    int len;
    int argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++; // Skip leading whitespace
        
        if (*cp == '\0') break; // Line was only whitespace

        start = cp;
        len = 1;
        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t')) {
            len++;
        }
        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }

    if (argnum == 0) { // No arguments were parsed
        for(int i = 0; i < MAXARGS + 1; i++) free(arglist[i]);
        free(arglist);
        return NULL;
    }

    arglist[argnum] = NULL;
    return arglist;
}
// Feature 2: Built-in Commands
// Return 1 if command is handled, otherwise return 0.
int handle_builtin(char** arglist)
{
    if (arglist == NULL || arglist[0] == NULL)
        return 0;

    // Built-in 1: exit
    if (strcmp(arglist[0], "exit") == 0)
    {
        printf("Exiting custom shell...\n");
        exit(EXIT_SUCCESS);
    }

    // Built-in 2: cd
    if (strcmp(arglist[0], "cd") == 0)
    {
        const char* directory = arglist[1];

        // cd without argument goes to HOME
        if (directory == NULL)
        {
            directory = getenv("HOME");
        }

        if (directory == NULL)
        {
            fprintf(stderr, "cd: HOME is not set\n");
        }
        else if (arglist[2] != NULL && arglist[2][0] != '\0')
        {
            fprintf(stderr, "cd: too many arguments\n");
        }
        else if (chdir(directory) == -1)
        {
            perror("cd");
        }

        return 1;
    }

    // Built-in 3: help
    if (strcmp(arglist[0], "help") == 0)
    {
        printf("\n===== Custom Shell Help =====\n");
        printf("exit           - Exit the shell\n");
        printf("cd <directory> - Change current directory\n");
        printf("cd             - Go to home directory\n");
        printf("help           - Display built-in commands\n");
        printf("jobs           - Display job information\n");
        printf("=============================\n\n");

        return 1;
    }

    // Built-in 4: jobs (placeholder)
    if (strcmp(arglist[0], "jobs") == 0)
    {
        printf("Job control not yet implemented.\n");
        return 1;
    }

    // Not a built-in command
    return 0;
}
