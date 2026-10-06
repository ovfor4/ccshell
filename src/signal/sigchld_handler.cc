#include "signal_handler.h"

#include <errno.h>

namespace ov4
{

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler([[maybe_unused]] int sig) 
{
    int errno_backup = errno;

    sigset_t prev;
    block_handler(&prev);

    int status, pid;
    safe_debug("SIGCHLD\n");

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        // signal'd or stopped or normally quit
        // print terminated/stopped message
        if (WIFSIGNALED(status) || WIFSTOPPED(status))
        {
            int jid = pid2jid(pid);
            safe_output("Job [", jid, "] (", pid, ") ");
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

        // handle job table
        // and save exit/signal code

        // stopped
        if (WIFSTOPPED(status))
        {
            safe_output("Process suspended PID: ", pid, "\n");
            sigset_t prev_inner;
            block_all(&prev_inner);
            job_suspend(pid);
            sigprocmask(SIG_SETMASK, &prev_inner, nullptr);
        }
        // other type of signal'd
        // or normally quit
        else {
            safe_output("Process terminated PID: ", pid, "\n");
            sigset_t prev_inner;
            block_all(&prev_inner);
            deletejob(pid);
            sigprocmask(SIG_SETMASK, &prev_inner, nullptr);

            // terminated normally
            // so that exit code should be available
            if (WIFEXITED(status))
            {
                safe_output("Exit code: ", WEXITSTATUS(status), "\n");
                if (pid == exit_required_pid)
                {
                    exit_code = WEXITSTATUS(status);
                    safe_logger("sigchld_handler: required PID detected: ", pid, "\n");
                }
            }
            // if signal'd
            else if (WIFSIGNALED(status))
            {
                safe_output("Signal code: ", WTERMSIG(status), "\n");
                if (pid == exit_required_pid)
                {
                    exit_code = SIGNAL_EXIT_CODE_BASE + WTERMSIG(status);
                    safe_logger("sigchld_handler: required PID detected: ", pid, "\n");
                }
            }

        }
    }

    sigprocmask(SIG_SETMASK, &prev, nullptr);

    errno = errno_backup;
    return;
}

}