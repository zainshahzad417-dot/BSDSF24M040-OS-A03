#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *history[HISTORY_SIZE] = {NULL};
static long total_commands = 0;
static int stored_count = 0;

void history_add(const char *command)
{
    if (command == NULL || command[0] == '\0')
        return;

    size_t length = strlen(command);

    char *copy = malloc(length + 1);

    if (copy == NULL) {
        perror("history: malloc");
        return;
    }

    memcpy(copy, command, length + 1);

    int index = (int)(total_commands % HISTORY_SIZE);

    free(history[index]);
    history[index] = copy;

    total_commands++;

    if (stored_count < HISTORY_SIZE)
        stored_count++;
}

void history_print(void)
{
    long first_number = total_commands - stored_count + 1;

    for (long n = first_number; n <= total_commands; n++) {
        int index = (int)((n - 1) % HISTORY_SIZE);

        if (history[index] != NULL)
            printf("%ld  %s\n", n, history[index]);
    }
}

const char *history_get(long number)
{
    long first_number = total_commands - stored_count + 1;

    if (number < first_number || number > total_commands)
        return NULL;

    int index = (int)((number - 1) % HISTORY_SIZE);

    return history[index];
}

void history_clear(void)
{
    for (int i = 0; i < HISTORY_SIZE; i++) {
        free(history[i]);
        history[i] = NULL;
    }

    total_commands = 0;
    stored_count = 0;
}
