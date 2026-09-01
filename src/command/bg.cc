#include "command/builtin.h"

#include "ground.h"

namespace ov4::command
{

void bg(char **argv)
{
    ov4::do_bgfg(argv);
}

}
