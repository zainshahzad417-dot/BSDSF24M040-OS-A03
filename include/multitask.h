#ifndef MULTITASK_H
#define MULTITASK_H

typedef int (*command_handler)(const char *command,
                               int background);

int multitask_execute(const char *line,
                      command_handler handler);

#endif
