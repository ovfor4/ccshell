#include "pipe.h"

#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>

#include "error.h"

namespace ov4
{

void self_pipe_init()
{
    int ret = pipe2(self_pipe_fd, O_NONBLOCK);
    if (ret != 0)
    {
        throw T_error(error_code::FAIL_TO_OPEN_PIPE, "Fail to open self pipe");
    }
}

/*
 * write a byte to self pipe
 * NB: you can ONLY determine that signal handler was called
 * but CANNOT determine the number of callings, or which signal was called
 * because when the pipe is full, nothing will be written
 */
int self_pipe_write()
{
    return write(self_pipe_fd[1], &SELF_PIPE_BYTE, 1);
}

}