#include "command/builtin.h"

namespace ov4::command
{

using namespace ov4;

void quit(char **argv)
{
    std::exit(0);
}

}
