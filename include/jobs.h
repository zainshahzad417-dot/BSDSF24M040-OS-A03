#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

void jobs_add(pid_t pid, const char *command);
void jobs_reap(void);
void jobs_print(void);

#endif
