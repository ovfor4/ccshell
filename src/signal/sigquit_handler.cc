#include "signal_handler.h"

#include <errno.h>

namespace ov4
{

void sigquit_handler([[maybe_unused]] int sig) 
{
    //printf("Terminating after receipt of SIGQUIT signal\n");
    safe_output("Terminating after receipt of SIGQUIT signal\n");
    // UNDERSCORE'd version should be safe
    // but exit() is unsafe
    _exit(1);
}

}