#define _POSIX_C_SOURCE 200809L

#include "shell.h"
#include "history.h"
#include "completion.h"
#include "io.h"
#include "jobs.h"
#include "multitask.h"

#include <readline/readline.h>
#include <readline/history.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Execute one command segment.
 * Built-ins run in the parent shell.
 * External commands use the Feature 5 I/O executor.
 */
static int execute_segment(const char *command, int background)
{
    if (background) {
        /*
         * A supervising child manages the external command
         * or pipeline. The main shell does not wait for it.
         */
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            int result = execute_io(command);
            _exit(result);
        }

        jobs_add(pid, command);
        return 0;
    }

    /*
     * Tokenize for built-in detection.
     * Our existing built-ins remain in the parent.
     */
    char **arglist = tokenize((char *)command);

    if (arglist == NULL)
        return 0;

    int result = 0;

    if (strcmp(arglist[0], "history") == 0) {
        history_print();
    }
    else if (!handle_builtin(arglist)) {
        result = execute_io(command);
    }

    // Free tokenized arguments.
    for (int i = 0; arglist[i] != NULL; i++)
        free(arglist[i]);

    free(arglist);

    return result;
}

int main(void)
{
    char *cmdline;

    completion_init();

    while (1) {
        /*
         * Reap finished background jobs before
         * displaying the next prompt.
         */
        jobs_reap();

        cmdline = readline(PROMPT);

        if (cmdline == NULL)
            break;

        char *input = cmdline;

        // Skip leading whitespace.
        while (isspace((unsigned char)*input))
            input++;

        if (*input == '\0') {
            free(cmdline);
            continue;
        }

        /*
         * Feature 3: Expand !n before adding
         * the command to history.
         */
        if (*input == '!') {
            char *endptr;
            errno = 0;

            long number = strtol(input + 1, &endptr, 10);

            while (isspace((unsigned char)*endptr))
                endptr++;

            if (errno != 0 ||
                endptr == input + 1 ||
                *endptr != '\0' ||
                number <= 0) {

                fprintf(stderr,
                        "history: invalid event number\n");

                free(cmdline);
                continue;
            }

            const char *previous =
                shell_history_get(number);

            if (previous == NULL) {
                fprintf(stderr,
                        "history: event %ld not found\n",
                        number);

                free(cmdline);
                continue;
            }

            char *expanded = strdup(previous);

            if (expanded == NULL) {
                perror("strdup");
                free(cmdline);
                continue;
            }

            free(cmdline);
            cmdline = expanded;
            input = cmdline;

            printf("%s\n", input);
        }

        // Preserve Feature 4 arrow-key history.
        add_history(input);

        // Preserve Feature 3 numbered history.
        history_add(input);

        // Feature 6: sequential and background commands.
        multitask_execute(input, execute_segment);

        free(cmdline);
    }

    history_clear();

    printf("\nShell exited.\n");
    return 0;
}
