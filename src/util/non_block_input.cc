#include "util/non_block_input.h"

#include <unistd.h>
#include <sys/select.h>

#include "pipe.h"

using namespace std;

namespace ov4
{


/*
 * read from both stdin, and self pipe
 * returns immediately as soon as stdin or pipe is "readable"
 * returns read()'s ret value
 * and set src to the the read source (stdin or pipe)
 */
int non_block_read_char(char *c, int *src)
{
    // get fd check range
    int max_fd;
    if (STDIN_FILENO > self_pipe_fd[0]) max_fd = STDIN_FILENO;
    else max_fd = self_pipe_fd[0];

    // monitor STDIN, and self pipe
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(STDIN_FILENO, &rset);
    FD_SET(self_pipe_fd[0], &rset);

    select(max_fd+1, &rset, nullptr, nullptr, nullptr);

    int ret = -1;
    if (FD_ISSET(self_pipe_fd[0], &rset))
    {
        ret = read(self_pipe_fd[0], c, 1);
        if (src != nullptr) *src = self_pipe_fd[0];
    }
    else if (FD_ISSET(STDIN_FILENO, &rset))
    {
        ret = read(STDIN_FILENO, c, 1);
        if (src != nullptr) *src = STDIN_FILENO;
    }
    return ret;
} 

}