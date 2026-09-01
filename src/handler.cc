#include "handler.h"

namespace ov4
{

extern char **environ;
sigset_t BLOCK_HANDLER;

void signal_init()
{
    /* Install the signal handlers */
    
    sigemptyset(&BLOCK_HANDLER);
    sigaddset(&BLOCK_HANDLER, SIGINT);
    sigaddset(&BLOCK_HANDLER, SIGTSTP);
    sigaddset(&BLOCK_HANDLER, SIGCHLD);
    
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
}

/*
 * Signal - wrapper for the sigaction function
 */

handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}
    
/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    int errno_backup = errno;

    sigset_t prev;
    block_handler(&prev);

    int status, pid;
    atomic_debug("SIGCHLD\n");

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        if (WIFSIGNALED(status) || WIFSTOPPED(status))
        {
            int jid = pid2jid(pid);
            atomic_print("Job [");
            atomic_print(jid);
            atomic_print("] (");
            atomic_print(pid);
            atomic_print(") ");
            if (!WIFSTOPPED(status) && WTERMSIG(status) == SIGINT)
                atomic_print("terminated");
            else if (WIFSTOPPED(status))
                atomic_print("stopped");
            else 
                atomic_print("idk");
            atomic_print(" by signal ");
            if (WIFSTOPPED(status))
                atomic_print(WSTOPSIG(status));
            else
                atomic_print(WTERMSIG(status));
            atomic_print("\n");
        } 

        if (WIFSTOPPED(status))
        {
            atomic_debug("suspended pid: ");
            atomic_debug(pid);
            atomic_debug("\n");
            sigset_t prev_inner;
            block_all(&prev_inner);
            job_suspend(jobs, pid);
            sigprocmask(SIG_SETMASK, &prev_inner, nullptr);
        }
        else {
            atomic_debug("terminated pid: ");
            atomic_debug(pid);
            sigset_t prev_inner;
            block_all(&prev_inner);
            deletejob(jobs, pid);
            sigprocmask(SIG_SETMASK, &prev_inner, nullptr);
        }
    }

    sigprocmask(SIG_SETMASK, &prev, nullptr);

    errno = errno_backup;
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    int errno_backup = errno;

    sigset_t prev;
    block_handler(&prev);

    int pid = fgpid(jobs);
    if (pid != 0)
    {
        kill(-pid, SIGINT);
    }

    sigprocmask(SIG_SETMASK, &prev, nullptr);

    errno = errno_backup;
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    int errno_backup = errno;

    sigset_t prev;
    block_handler(&prev);

    int pid = fgpid(jobs);
    if (pid != 0)
    {
        kill(-pid, SIGTSTP);
    }

    sigprocmask(SIG_SETMASK, &prev, nullptr);

    errno = errno_backup;
    return;
}

int block_all(sigset_t *prev)
{
    sigset_t set;
    sigfillset(&set);
    return sigprocmask(SIG_SETMASK, &set, prev);
}

int block_handler(sigset_t *prev)
{
    return sigprocmask(SIG_BLOCK, &BLOCK_HANDLER, prev);
}

void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}

}
