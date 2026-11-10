#define _POSIX_C_SOURCE 200809L

#include "variables.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Variable {
    char *name;
    char *value;
    struct Variable *next;
} Variable;

static Variable *variables = NULL;

static int name_start(char c)
{
    return isalpha((unsigned char)c) || c == '_';
}

static int name_char(char c)
{
    return isalnum((unsigned char)c) || c == '_';
}

static Variable *find_variable(const char *name)
{
    for (Variable *v = variables; v; v = v->next) {
        if (strcmp(v->name, name) == 0)
            return v;
    }
    return NULL;
}

static const char *get_value(const char *name)
{
    Variable *v = find_variable(name);

    if (v)
        return v->value;

    // Also allow existing environment variables.
    const char *env = getenv(name);
    return env ? env : "";
}

int variables_assign(const char *command)
{
    if (!command)
        return 0;

    char *copy = strdup(command);
    if (!copy)
        return -1;

    char *text = copy;

    while (isspace((unsigned char)*text))
        text++;

    char *end = text + strlen(text);

    while (end > text &&
           isspace((unsigned char)end[-1]))
        *--end = '\0';

    char *equal = strchr(text, '=');

    if (!equal) {
        free(copy);
        return 0;
    }

    // Assignment must be the first command word.
    for (char *p = text; p < equal; p++) {
        if (isspace((unsigned char)*p)) {
            free(copy);
            return 0;
        }
    }

    if (!name_start(text[0])) {
        fprintf(stderr, "variable: invalid name\n");
        free(copy);
        return -1;
    }

    for (char *p = text; p < equal; p++) {
        if (!name_char(*p)) {
            fprintf(stderr, "variable: invalid name\n");
            free(copy);
            return -1;
        }
    }

    *equal = '\0';

    char *value = equal + 1;
    size_t length = strlen(value);

    // Remove matching surrounding quotation marks.
    if (length > 0 &&
        (value[0] == '"' || value[0] == '\'')) {
        char quote = value[0];

        if (length < 2 || value[length - 1] != quote) {
            fprintf(stderr, "variable: unmatched quote\n");
            free(copy);
            return -1;
        }

        value[length - 1] = '\0';
        value++;
    }
    else {
        // Unquoted assignment cannot contain spaces.
        for (char *p = value; *p; p++) {
            if (isspace((unsigned char)*p)) {
                fprintf(stderr,
                        "variable: use quotes for spaces\n");
                free(copy);
                return -1;
            }
        }
    }

    Variable *existing = find_variable(text);
    char *new_value = strdup(value);

    if (!new_value) {
        perror("variable: strdup");
        free(copy);
        return -1;
    }

    if (existing) {
        free(existing->value);
        existing->value = new_value;
    }
    else {
        Variable *node = malloc(sizeof(*node));

        if (!node) {
            perror("variable: malloc");
            free(new_value);
            free(copy);
            return -1;
        }

        node->name = strdup(text);

        if (!node->name) {
            free(node);
            free(new_value);
            free(copy);
            return -1;
        }

        node->value = new_value;
        node->next = variables;
        variables = node;
    }

    free(copy);
    return 1;
}

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} Buffer;

static int append_char(Buffer *b, char c)
{
    if (b->length + 2 > b->capacity) {
        size_t capacity = b->capacity ? b->capacity * 2 : 128;

        char *new_data = realloc(b->data, capacity);

        if (!new_data)
            return -1;

        b->data = new_data;
        b->capacity = capacity;
    }

    b->data[b->length++] = c;
    b->data[b->length] = '\0';
    return 0;
}

static int append_value(Buffer *b, const char *value,
                        char quote)
{
    /*
     * Protect expanded values from being split into
     * multiple arguments by the existing I/O parser.
     */
    if (quote == '\0' && append_char(b, '"') < 0)
        return -1;

    for (const char *p = value; *p; p++) {
        if (*p == '"' || *p == '\\') {
            if (append_char(b, '\\') < 0)
                return -1;
        }

        if (append_char(b, *p) < 0)
            return -1;
    }

    if (quote == '\0' && append_char(b, '"') < 0)
        return -1;

    return 0;
}

char *variables_expand(const char *command)
{
    if (!command)
        return NULL;

    Buffer b = {0};
    char quote = '\0';

    for (size_t i = 0; command[i]; i++) {
        char c = command[i];

        // Preserve escaped characters.
        if (c == '\\' && quote != '\'' && command[i + 1]) {
            if (append_char(&b, c) < 0 ||
                append_char(&b, command[++i]) < 0)
                goto fail;
            continue;
        }

        if (c == '\'' || c == '"') {
            if (quote == '\0')
                quote = c;
            else if (quote == c)
                quote = '\0';
        }

        // Expand variables outside single quotes.
        if (c == '$' && quote != '\'' &&
            name_start(command[i + 1])) {

            size_t start = ++i;

            while (name_char(command[i]))
                i++;

            size_t length = i - start;

            char *name = strndup(command + start, length);

            if (!name)
                goto fail;

            const char *value = get_value(name);
            int result = append_value(&b, value, quote);

            free(name);

            if (result < 0)
                goto fail;

            i--;
            continue;
        }

        if (append_char(&b, c) < 0)
            goto fail;
    }

    if (!b.data)
        return strdup("");

    return b.data;

fail:
    perror("variable: memory allocation");
    free(b.data);
    return NULL;
}

void variables_print(void)
{
    for (Variable *v = variables; v; v = v->next)
        printf("%s=%s\n", v->name, v->value);
}

void variables_clear(void)
{
    while (variables) {
        Variable *next = variables->next;

        free(variables->name);
        free(variables->value);
        free(variables);

        variables = next;
    }
}
