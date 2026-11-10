#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_SIZE 20

void history_add(const char *command);
void history_print(void);
const char *shell_history_get(long number);
void history_clear(void);

#endif
