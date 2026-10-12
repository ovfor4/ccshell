#include "signal_handler.h"

#include "line/getter.h"

namespace ov4
{

void sigwinch_handler([[maybe_unused]] int sig)
{
    windows_size_changed = true;
}

}