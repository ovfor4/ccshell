#include "command/builtin.h"

#include "job.h"

namespace ov4::command
{

using namespace ov4;

void jobs([[maybe_unused]] char **argv)
{
    listjobs();
}

}
