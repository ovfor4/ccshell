#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "global.h"
#include "util/io.h"
#include "job.h"
#include "handler.h"

namespace ov4
{

extern pid_t shell_pgid;
extern int tty_fd;

void waitfg(pid_t pid);
void do_bgfg(char **argv);
    

}
