#include "multitask.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int run_segment(char *segment,
                       int background,
                       command_handler handler)
{
    // Remove leading spaces.
    while (isspace((unsigned char)*segment))
        segment++;

    // Remove trailing spaces.
    size_t length = strlen(segment);

    while (length > 0 &&
           isspace((unsigned char)segment[length - 1])) {
        segment[--length] = '\0';
    }

    if (length == 0)
        return 0;

    return handler(segment, background);
}

int multitask_execute(const char *line,
                      command_handler handler)
{
    if (line == NULL || handler == NULL)
        return 1;

    size_t length = strlen(line);

    // Check quotation marks before executing anything.
    char quote = '\0';

    for (size_t i = 0; i < length; i++) {
        if (line[i] == '\\' && quote != '\'') {
            if (i + 1 < length)
                i++;
            continue;
        }

        if (quote != '\0') {
            if (line[i] == quote)
                quote = '\0';
        }
        else if (line[i] == '\'' || line[i] == '"') {
            quote = line[i];
        }
    }

    if (quote != '\0') {
        fprintf(stderr, "shell: unmatched quote\n");
        return 1;
    }

    char *buffer = malloc(length + 1);

    if (buffer == NULL) {
        perror("malloc");
        return 1;
    }

    size_t used = 0;
    quote = '\0';
    int result = 0;

    for (size_t i = 0; i <= length; i++) {
        char ch = line[i];

        // Preserve escaped characters.
        if (ch == '\\' && quote != '\'' && i < length) {
            buffer[used++] = ch;

            if (i + 1 < length)
                buffer[used++] = line[++i];

            continue;
        }

        if (quote != '\0') {
            if (ch == quote)
                quote = '\0';
        }
        else if (ch == '\'' || ch == '"') {
            quote = ch;
        }

        // Split only outside quotation marks.
        if ((ch == ';' || ch == '&' || ch == '\0') &&
            quote == '\0') {

            buffer[used] = '\0';

            int background = (ch == '&');

            result = run_segment(
                buffer, background, handler
            );

            used = 0;
        }
        else {
            buffer[used++] = ch;
        }
    }

    free(buffer);
    return result;
}
