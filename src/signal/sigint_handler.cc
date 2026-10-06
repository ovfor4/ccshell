#include "signal_handler.h"

#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

namespace ov4
{

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler([[maybe_unused]] int sig) 
{
    int errno_backup = errno;

    sigset_t prev;
    block_handler(&prev);

    int pid = fgpid();
    if (pid != 0)
    {
        kill(-pid, SIGINT);
    } else {
        tcflush(STDIN_FILENO, TCIFLUSH);
        ioctl(STDIN_FILENO, TIOCSTI, "\n");
    }

    sigprocmask(SIG_SETMASK, &prev, nullptr);

    errno = errno_backup;
    return;
}

}