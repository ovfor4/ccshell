#include "signal_handler.h"

#include <errno.h>

namespace ov4
{

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler([[maybe_unused]] int sig) 
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

}