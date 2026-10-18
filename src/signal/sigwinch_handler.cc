#include "signal_handler.h"

#include "line/line_editor.h"

namespace ov4
{

void sigwinch_handler([[maybe_unused]] int sig)
{
    windows_size_changed = true;
}

}