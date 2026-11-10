#include "completion.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "shell.h"
#include "history.h"
#include <ctype.h>
#include <errno.h>

int main(void)
{
    completion_init();
    char *cmdline;
    char **arglist;

    while ((cmdline = readline(PROMPT)) != NULL)
    {
        // Ignore leading spaces
        char *input = cmdline;

        while (isspace((unsigned char)*input))
            input++;

        // Ignore empty commands
        if (*input == '\0') {
            free(cmdline);
            continue;
        }

        // Feature 3: Handle !n before history storage
        if (*input == '!')
        {
            char *endptr;
            errno = 0;

            long number = strtol(input + 1, &endptr, 10);

            while (isspace((unsigned char)*endptr))
                endptr++;

            if (errno != 0 ||
                endptr == input + 1 ||
                *endptr != '\0' ||
                number <= 0)
            {
                fprintf(stderr, "history: invalid event number\n");
                free(cmdline);
                continue;
            }

            const char *previous = shell_history_get(number);

            if (previous == NULL)
            {
                fprintf(stderr,
                        "history: event %ld not found\n",
                        number);
                free(cmdline);
                continue;
            }

            // Copy before adding, because history may be full
            char *expanded = malloc(strlen(previous) + 1);

            if (expanded == NULL) {
                perror("history: malloc");
                free(cmdline);
                continue;
            }

            strcpy(expanded, previous);
            free(cmdline);
            cmdline = expanded;
            input = cmdline;

            printf("%s\n", input);
        }

        // Store resolved command in history
        add_history(input);
        history_add(input);

        // Tokenize the command
        arglist = tokenize(input);

        if (arglist != NULL)
        {
            // Feature 3: history built-in
            if (strcmp(arglist[0], "history") == 0)
            {
                history_print();
            }
            // Feature 2: existing built-ins
            else if (!handle_builtin(arglist))
            {
                // External command
                execute(arglist);
            }

            // Free tokenized arguments
            for (int i = 0; arglist[i] != NULL; i++)
                free(arglist[i]);

            free(arglist);
        }

        free(cmdline);
    }

    history_clear();
    printf("\nShell exited.\n");

    return 0;
}
