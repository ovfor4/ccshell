#include "signal_handler.h"

#include <errno.h>

namespace ov4
{

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

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal([[maybe_unused]] int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
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

}