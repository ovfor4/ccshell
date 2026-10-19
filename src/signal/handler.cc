#include "signal_handler.h"

#include <print>
#include <signal.h>
#include <cstdio>

using namespace std;

namespace ov4
{

void process_signal()
{
    /*
     * do NOT swap these statements
     * otherwise, if process then unset flag, and another signal delivered
     * it won't be processed
     */
    if (check_signal_delivered(SIGCHLD))
    {
        unset_signal_delivered(SIGCHLD);
        process_sigchld();
    }
}

void process_sigchld()
{
    print("hi\r\n");
    fflush(stdout);
}

}