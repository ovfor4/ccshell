#pragma once

#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "util/io.h"
#include "job.h"

namespace ov4
{

typedef void handler_t(int);

void signal_init();
handler_t *Signal(int signum, handler_t *handler);
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigquit_handler(int sig);
int block_all(sigset_t *prev);
int block_handler(sigset_t *prev);

}
