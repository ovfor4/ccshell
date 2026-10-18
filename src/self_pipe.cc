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

}