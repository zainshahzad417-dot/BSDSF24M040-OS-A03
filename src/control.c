#define _POSIX_C_SOURCE 200809L

#include "control.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#define MAX_BLOCK_LINES 128

typedef enum {
    EXPECT_THEN,
    IN_THEN,
    IN_ELSE
} IfState;

static char *trim(char *s)
{
    while (isspace((unsigned char)*s))
        s++;

    size_t length = strlen(s);

    while (length > 0 &&
           isspace((unsigned char)s[length - 1])) {
        s[--length] = '\0';
    }

    return s;
}

static void free_lines(char **lines, int count)
{
    for (int i = 0; i < count; i++)
        free(lines[i]);
}

static int store_line(char **lines,
                      int *count,
                      const char *text)
{
    if (*count >= MAX_BLOCK_LINES) {
        fprintf(stderr,
                "if: too many commands in block\n");
        return -1;
    }

    lines[*count] = strdup(text);

    if (lines[*count] == NULL) {
        perror("if: strdup");
        return -1;
    }

    (*count)++;
    return 0;
}

/*
 * Execute a multi-line if-then-else-fi block.
 *
 * The handler is the existing shell executor.
 * It returns the command's exit status.
 */
int run_if_block(const char *first_line,
                 command_handler handler)
{
    if (first_line == NULL || handler == NULL)
        return 1;

    // Skip the "if" keyword.
    const char *condition = first_line + 2;

    while (isspace((unsigned char)*condition))
        condition++;

    if (*condition == '\0') {
        fprintf(stderr,
                "if: missing condition\n");
        return 2;
    }

    char *then_lines[MAX_BLOCK_LINES] = {0};
    char *else_lines[MAX_BLOCK_LINES] = {0};

    int then_count = 0;
    int else_count = 0;

    IfState state = EXPECT_THEN;
    int complete = 0;
    int error = 0;

    /*
     * Read the complete control structure before
     * executing either branch.
     */
    while (!complete) {
        char *line = readline("if> ");

        if (line == NULL) {
            fprintf(stderr,
                    "if: unexpected end of input\n");
            error = 1;
            break;
        }

        char *text = trim(line);

        if (*text == '\0') {
            free(line);
            continue;
        }

        // Save entered lines for Readline arrow navigation.
        add_history(text);

        if (state == EXPECT_THEN) {
            if (strcmp(text, "then") == 0) {
                state = IN_THEN;
            }
            else {
                fprintf(stderr,
                        "if: expected 'then'\n");
                error = 1;
            }
        }
        else if (strcmp(text, "fi") == 0) {
            complete = 1;
        }
        else if (strcmp(text, "else") == 0) {
            if (state == IN_ELSE) {
                fprintf(stderr,
                        "if: duplicate 'else'\n");
                error = 1;
            }
            else {
                state = IN_ELSE;
            }
        }
        else if (strcmp(text, "then") == 0) {
            fprintf(stderr,
                    "if: unexpected 'then'\n");
            error = 1;
        }
        else if (strncmp(text, "if ", 3) == 0) {
            fprintf(stderr,
                    "if: nested blocks not supported\n");
            error = 1;
        }
        else if (state == IN_THEN) {
            if (store_line(then_lines,
                           &then_count, text) != 0)
                error = 1;
        }
        else {
            if (store_line(else_lines,
                           &else_count, text) != 0)
                error = 1;
        }

        free(line);

        if (error)
            break;
    }

    int result = 2;

    if (!error && complete) {
        /*
         * Execute the condition using the existing
         * fork-exec-wait command execution path.
         */
        int condition_status = handler(condition, 0);

        char **selected_lines;
        int selected_count;

        if (condition_status == 0) {
            selected_lines = then_lines;
            selected_count = then_count;
        }
        else {
            selected_lines = else_lines;
            selected_count = else_count;
        }

        result = 0;

        // Run selected commands sequentially.
        for (int i = 0; i < selected_count; i++) {
            result = multitask_execute(
                selected_lines[i],
                handler
            );
        }
    }

    free_lines(then_lines, then_count);
    free_lines(else_lines, else_count);

    return result;
}
