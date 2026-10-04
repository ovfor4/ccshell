#include "eval.h"

#include <print>
#include <unistd.h>

using namespace std;

namespace ov4
{

/*
 * print exec error message
 */
void eval_err(int err)
{
    switch (err)
    {
        case ENOTDIR:
            println(": A component of the path prefix is not a directory.");
            _exit(errno);
            break;

        case ENAMETOOLONG:
            println(": A component of a pathname exceeded 255 characters, or an entire path name exceeded 1023 characters.");
            _exit(errno);
            break;

        case ENOENT:
            println(": The new process file does not exist.");
            _exit(errno);
            break;

        case EACCES:
            println(": The new process file mode denies execute permission.");
            _exit(errno);
            break;

        default:
            println(": Error, code: {}", errno);
            _exit(errno);
    }
}

}