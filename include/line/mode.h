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

void clear_below();

std::string readline();

void editor_exit(int code);

}
