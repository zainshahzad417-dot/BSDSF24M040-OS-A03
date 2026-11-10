#ifndef VARIABLES_H
#define VARIABLES_H

// Returns 1 for an assignment, 0 if not an
// assignment, and -1 for an invalid assignment.
int variables_assign(const char *command);

// Returns a newly allocated expanded command.
char *variables_expand(const char *command);

// Prints stored shell variables.
void variables_print(void);

// Releases all variable memory.
void variables_clear(void);

#endif
