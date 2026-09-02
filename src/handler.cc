#include "handler.h"

#include <termios.h>
#include <sys/ioctl.h>

namespace ov4
{

sigset_t 
    block_sig_TTOU,
    block_job,
    block_io;

void signal_init()
{
    /* Install the signal handlers */
    
    sigemptyset(&block_job);
    sigemptyset(&block_sig_TTOU);
    sigemptyset(&block_io);

    sigaddset(&block_job, SIGINT);
    sigaddset(&block_job, SIGTSTP);
    sigaddset(&block_job, SIGCHLD);

    sigaddset(&block_sig_TTOU, SIGTTOU);

    sigaddset(&block_io, SIGTTOU);
    sigaddset(&block_io, SIGTTIN);

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
    safe_debug("SIGCHLD\n");

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        if (WIFSIGNALED(status) || WIFSTOPPED(status))
        {
            int jid = pid2jid(pid);
            safe_print("Job [");
            safe_print(jid);
            safe_print("] (");
            safe_print(pid);
            safe_print(") ");
            if (!WIFSTOPPED(status) && WTERMSIG(status) == SIGINT)
                safe_print("terminated");
            else if (WIFSTOPPED(status))
                safe_print("stopped");
            else 
                safe_print("idk");
            safe_print(" by signal ");
            if (WIFSTOPPED(status))
                safe_print(WSTOPSIG(status));
            else
                safe_print(WTERMSIG(status));
            safe_print("\n");
        } 

        if (WIFSTOPPED(status))
        {
            safe_debug("suspended pid: ");
            safe_debug(pid);
            safe_debug("\n");
            sigset_t prev_inner;
            block_all(&prev_inner);
            job_suspend(pid);
            sigprocmask(SIG_SETMASK, &prev_inner, nullptr);
        }
        else {
            safe_debug("terminated pid: ");
            safe_debug(pid);
            sigset_t prev_inner;
            block_all(&prev_inner);
            deletejob(pid);
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

    int pid = fgpid();
    if (pid != 0)
    {
        kill(-pid, SIGINT);
    } else {
        char *buf = "\n";
        tcflush(STDIN_FILENO, TCIFLUSH);
        ioctl(STDIN_FILENO, TIOCSTI, buf);
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

    int pid = fgpid();
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
    return sigprocmask(SIG_BLOCK, &block_job, prev);
}

void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}

}
