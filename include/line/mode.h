#pragma once

#include <termios.h>
#include <string>

#include "line/type.h"

namespace ov4
{

inline termios attr;
inline bool raw_enabled = false;

void disable_raw();
void enable_raw();



}
