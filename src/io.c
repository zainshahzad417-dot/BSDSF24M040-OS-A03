#define _POSIX_C_SOURCE 200809L

#include "io.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_TOKENS 128
#define MAX_COMMANDS 16

typedef enum {
    WORD,
    INPUT,
    OUTPUT,
    PIPE
} TokenType;

typedef struct {
    TokenType type;
    char *text;
} Token;

typedef struct {
    char *argv[MAX_TOKENS + 1];
    int argc;
    char *input_file;
    char *output_file;
} Command;

static void free_tokens(Token *tokens, int count)
{
    for (int i = 0; i < count; i++)
        free(tokens[i].text);
}

static int add_token(Token *tokens, int *count,
                     TokenType type, char *text)
{
    if (*count >= MAX_TOKENS) {
        fprintf(stderr, "shell: too many tokens\n");
        free(text);
        return -1;
    }

    tokens[*count].type = type;
    tokens[*count].text = text;
    (*count)++;
    return 0;
}

/*
 * Recognize:
 *   ordinary words
 *   < > |
 *   quoted strings
 *   backslash escapes
 *
 * Operators inside quotes are treated as text.
 */
static int lex_line(const char *line, Token *tokens)
{
    int count = 0;
    const char *p = line;

    while (*p) {
        while (isspace((unsigned char)*p))
            p++;

        if (!*p)
            break;

        TokenType type;

        if (*p == '<')
            type = INPUT;
        else if (*p == '>')
            type = OUTPUT;
        else if (*p == '|')
            type = PIPE;
        else
            type = WORD;

        if (type != WORD) {
            if (add_token(tokens, &count, type, NULL) < 0)
                goto fail;

            p++;
            continue;
        }

        char *word = malloc(strlen(p) + 1);
        if (!word) {
            perror("malloc");
            goto fail;
        }

        size_t length = 0;
        char quote = '\0';

        while (*p) {
            if (quote) {
                if (*p == quote) {
                    quote = '\0';
                    p++;
                }
                else if (*p == '\\' &&
                         quote == '"' && p[1]) {
                    p++;
                    word[length++] = *p++;
                }
                else {
                    word[length++] = *p++;
                }
            }
            else if (*p == '\'' || *p == '"') {
                quote = *p++;
            }
            else if (*p == '\\' && p[1]) {
                p++;
                word[length++] = *p++;
            }
            else if (isspace((unsigned char)*p) ||
                     *p == '<' || *p == '>' || *p == '|') {
                break;
            }
            else {
                word[length++] = *p++;
            }
        }

        if (quote) {
            fprintf(stderr, "shell: unmatched quote\n");
            free(word);
            goto fail;
        }

        word[length] = '\0';

        if (add_token(tokens, &count, WORD, word) < 0)
            goto fail;
    }

    return count;

fail:
    free_tokens(tokens, count);
    return -1;
}

static int parse_commands(Token *tokens, int count,
                          Command *commands)
{
    int current = 0;

    memset(commands, 0, sizeof(Command) * MAX_COMMANDS);

    for (int i = 0; i < count; i++) {
        Token *t = &tokens[i];

        if (t->type == PIPE) {
            if (commands[current].argc == 0 ||
                current + 1 >= MAX_COMMANDS) {
                fprintf(stderr, "shell: invalid pipe\n");
                return -1;
            }

            current++;
            continue;
        }

        if (t->type == INPUT || t->type == OUTPUT) {
            if (i + 1 >= count ||
                tokens[i + 1].type != WORD ||
                tokens[i + 1].text[0] == '\0') {
                fprintf(stderr,
                        "shell: redirection needs a filename\n");
                return -1;
            }

            char *filename = tokens[++i].text;

            if (t->type == INPUT) {
                if (commands[current].input_file) {
                    fprintf(stderr,
                            "shell: duplicate input redirection\n");
                    return -1;
                }

                commands[current].input_file = filename;
            }
            else {
                if (commands[current].output_file) {
                    fprintf(stderr,
                            "shell: duplicate output redirection\n");
                    return -1;
                }

                commands[current].output_file = filename;
            }

            continue;
        }

        Command *cmd = &commands[current];

        if (cmd->argc >= MAX_TOKENS) {
            fprintf(stderr, "shell: too many arguments\n");
            return -1;
        }

        cmd->argv[cmd->argc++] = t->text;
        cmd->argv[cmd->argc] = NULL;
    }

    if (commands[current].argc == 0) {
        fprintf(stderr, "shell: missing command\n");
        return -1;
    }

    return current + 1;
}

static void setup_redirection(const Command *cmd)
{
    int fd;

    if (cmd->input_file != NULL) {
        fd = open(cmd->input_file, O_RDONLY);

        if (fd < 0) {
            perror(cmd->input_file);
            _exit(1);
        }

        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("dup2 input");
            close(fd);
            _exit(1);
        }

        close(fd);
    }

    if (cmd->output_file != NULL) {
        fd = open(cmd->output_file,
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd < 0) {
            perror(cmd->output_file);
            _exit(1);
        }

        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2 output");
            close(fd);
            _exit(1);
        }

        close(fd);
    }
}

static int run_commands(Command *commands, int count)
{
    pid_t children[MAX_COMMANDS];
    int started = 0;
    int previous_read = -1;
    int failed = 0;

    for (int i = 0; i < count; i++) {
        int pipefd[2] = {-1, -1};

        if (i < count - 1) {
            if (pipe(pipefd) < 0) {
                perror("pipe");
                failed = 1;
                break;
            }
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");

            if (pipefd[0] >= 0)
                close(pipefd[0]);

            if (pipefd[1] >= 0)
                close(pipefd[1]);

            failed = 1;
            break;
        }

        if (pid == 0) {
            // Child process

            if (previous_read >= 0) {
                if (dup2(previous_read, STDIN_FILENO) < 0) {
                    perror("dup2 pipe input");
                    _exit(1);
                }
            }

            if (pipefd[1] >= 0) {
                if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
                    perror("dup2 pipe output");
                    _exit(1);
                }
            }

            if (previous_read >= 0)
                close(previous_read);

            if (pipefd[0] >= 0)
                close(pipefd[0]);

            if (pipefd[1] >= 0)
                close(pipefd[1]);

            // Explicit < or > redirection
            setup_redirection(&commands[i]);

            execvp(commands[i].argv[0], commands[i].argv);

            perror(commands[i].argv[0]);
            _exit(127);
        }

        // Parent process
        children[started++] = pid;

        if (previous_read >= 0)
            close(previous_read);

        if (pipefd[1] >= 0)
            close(pipefd[1]);

        previous_read = pipefd[0];
    }

    if (previous_read >= 0)
        close(previous_read);

    int last_status = 0;

    for (int i = 0; i < started; i++) {
        int status;
        pid_t result;

        do {
            result = waitpid(children[i], &status, 0);
        } while (result < 0 && errno == EINTR);

        if (result < 0) {
            perror("waitpid");
            failed = 1;
        }
        else if (i == started - 1) {
            if (WIFEXITED(status))
                last_status = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                last_status = 128 + WTERMSIG(status);
        }
    }

    return failed ? 1 : last_status;
}

int execute_io(const char *command_line)
{
    Token tokens[MAX_TOKENS] = {0};

    int token_count = lex_line(command_line, tokens);

    if (token_count < 0)
        return 1;

    if (token_count == 0)
        return 0;

    Command *commands = calloc(MAX_COMMANDS, sizeof(Command));

    if (!commands) {
        perror("calloc");
        free_tokens(tokens, token_count);
        return 1;
    }

    int command_count =
        parse_commands(tokens, token_count, commands);

    int result = 1;

    if (command_count > 0)
        result = run_commands(commands, command_count);

    free(commands);
    free_tokens(tokens, token_count);

    return result;
}
