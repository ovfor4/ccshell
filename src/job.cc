#include "job.h"

namespace ov4
{

struct job_t _jobs[MAXJOBS];
int nextjid = 1;

void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *job) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
		clearjob(&job[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *job) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (job[i].jid > max)
	    max = job[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int addjob(pid_t pid, int state, char *cmdline, struct job_t *job)
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (job[i].pid == 0) {
	    job[i].pid = pid;
	    job[i].state = state;
	    job[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(job[i].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] %d %s\n", job[i].jid, job[i].pid, job[i].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(pid_t pid, struct job_t *job) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (job[i].pid == pid) {
	    clearjob(&job[i]);
	    nextjid = maxjid(job)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *job) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (job[i].state == FG)
	    return job[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(pid_t pid, struct job_t *job) {
    int i;

    if (pid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (job[i].pid == pid)
	    return &job[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(int jid, struct job_t *job)
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (job[i].jid == jid)
	    return &job[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid, struct job_t *job)
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (job[i].pid == pid) {
            return job[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *job) 
{
    int i;
    
    for (i = 0; i < MAXJOBS; i++) {
	if (job[i].pid != 0) {
	    printf("[%d] (%d) ", job[i].jid, job[i].pid);
	    switch (job[i].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("listjobs: Internal error: job[%d].state=%d ", 
			   i, job[i].state);
	    }
	    printf("%s", job[i].cmdline);
	}
    }
}

bool job_suspend(pid_t pid, struct job_t *job) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (job[i].pid == pid)
	{
        job[i].state = ST;
        return true;
    }
    return false;
}

}
