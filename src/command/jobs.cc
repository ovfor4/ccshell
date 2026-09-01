#include "command/builtin.h"

#include "job.h"

namespace ov4::command
{

using namespace ov4;

void jobs(char **argv)
{
    listjobs(ov4::jobs);
}

}
