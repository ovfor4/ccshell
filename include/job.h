#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "global.h"

#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */


namespace ov4 
{

extern struct job_t _jobs[];
extern int nextjid;


/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

enum job_status
{
    UNDEF,
    FG,
    BG,
    ST
};
   
struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};

void clearjob(struct job_t *job = _jobs);
void initjobs(struct job_t *job = _jobs);
int maxjid(struct job_t *job = _jobs);
int addjob(pid_t pid, int state, char *cmdline, struct job_t *job = _jobs);
int deletejob(pid_t pid, struct job_t *job = _jobs);
pid_t fgpid(struct job_t *job = _jobs);
struct job_t *getjobpid(pid_t pid, struct job_t *job = _jobs);
struct job_t *getjobjid(int jid, struct job_t *job = _jobs);
int pid2jid(pid_t pid, struct job_t *job = _jobs);
void listjobs(struct job_t *job = _jobs);
bool job_suspend(pid_t pid, struct job_t *job = _jobs);

}
