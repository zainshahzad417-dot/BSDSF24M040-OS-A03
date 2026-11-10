#define _POSIX_C_SOURCE 200809L

#include "jobs.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_JOBS 64

typedef struct {
    int id;
    pid_t pid;
    char *command;
} Job;

static Job jobs[MAX_JOBS];
static int next_job_id = 1;

void jobs_add(pid_t pid, const char *command)
{

    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid == 0) {
            char *copy = strdup(command);

            if (copy == NULL) {
                perror("jobs: strdup");
                return;
            }

            jobs[i].id = next_job_id++;
            jobs[i].pid = pid;
            jobs[i].command = copy;

            printf("[%d] PID %ld started: %s\n",
                   jobs[i].id,
                   (long)pid,
                   command);
            fflush(stdout);
            return;
        }
    }

    fprintf(stderr, "jobs: job table is full\n");
}

void jobs_reap(void)
{
    int status;
    pid_t pid;

    while (1) {
        pid = waitpid(-1, &status, WNOHANG);

        if (pid > 0) {
            for (int i = 0; i < MAX_JOBS; i++) {
                if (jobs[i].pid == pid) {
                    free(jobs[i].command);
                    jobs[i].command = NULL;
                    jobs[i].pid = 0;
                    jobs[i].id = 0;
                    break;
                }
            }
        }
        else if (pid == 0) {
            break;
        }
        else if (errno == EINTR) {
            continue;
        }
        else {
            break;
        }
    }
}

void jobs_print(void)
{
    jobs_reap();

    int count = 0;

    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid != 0) {
            printf("[%d] PID %ld Running %s\n",
                   jobs[i].id,
                   (long)jobs[i].pid,
                   jobs[i].command);
            count++;
        }
    }

    if (count == 0)
        printf("No active background jobs.\n");
}
