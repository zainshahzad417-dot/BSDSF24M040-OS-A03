#define _POSIX_C_SOURCE 200809L

#include "completion.h"
#include <readline/readline.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

static const char *builtins[] = {
    "cd", "exit", "help", "jobs", "history", NULL
};

static char *path_copy = NULL;
static char *saveptr = NULL;
static char *directory = NULL;
static DIR *dir = NULL;
static int builtin_index = 0;

static void reset_generator(void)
{
    if (dir != NULL) {
        closedir(dir);
        dir = NULL;
    }

    free(path_copy);
    path_copy = NULL;
    saveptr = NULL;
    directory = NULL;
    builtin_index = 0;
}

static char *command_generator(const char *text, int state)
{
    if (state == 0) {
        reset_generator();

        const char *path = getenv("PATH");
        path_copy = strdup(path != NULL ? path : "");
        if (path_copy == NULL)
            return NULL;
    }

    size_t len = strlen(text);

    // First complete built-in commands.
    while (builtins[builtin_index] != NULL) {
        const char *name = builtins[builtin_index++];

        if (strncmp(name, text, len) == 0)
            return strdup(name);
    }

    // Then search executable programs in PATH.
    while (1) {
        if (dir == NULL) {
            if (directory == NULL)
                directory = strtok_r(path_copy, ":", &saveptr);
            else
                directory = strtok_r(NULL, ":", &saveptr);

            if (directory == NULL) {
                reset_generator();
                return NULL;
            }

            dir = opendir(directory);
            if (dir == NULL)
                continue;
        }

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {
            if (strncmp(entry->d_name, text, len) != 0)
                continue;

            size_t needed = strlen(directory)
                          + strlen(entry->d_name) + 2;

            char *fullpath = malloc(needed);
            if (fullpath == NULL)
                continue;

            snprintf(fullpath, needed, "%s/%s",
                     directory, entry->d_name);

            struct stat st;
            int valid = stat(fullpath, &st) == 0
                     && S_ISREG(st.st_mode)
                     && access(fullpath, X_OK) == 0;

            free(fullpath);

            if (valid)
                return strdup(entry->d_name);
        }

        closedir(dir);
        dir = NULL;
    }
}

static char **shell_completion(const char *text,
                               int start, int end)
{
    (void)end;

    // For command arguments, use Readline's
    // normal filename completion.
    for (int i = 0; i < start; i++) {
        if (rl_line_buffer[i] != ' ' &&
            rl_line_buffer[i] != '\t')
            return NULL;
    }

    // A path like ./program uses filename completion.
    if (strchr(text, '/') != NULL)
        return NULL;

    // For the first word, try command completion.
    char **matches = rl_completion_matches(
        text, command_generator
    );

    if (matches != NULL)
        return matches;

    // If no command matches, fall back to filenames.
    return NULL;
}

void completion_init(void)
{
    rl_attempted_completion_function = shell_completion;
}
