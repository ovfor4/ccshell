#pragma once

#include <termios.h>
#include <string>

using namespace std; // TODO: remove

namespace ov4
{

inline termios attr;

void disable_raw();
void enable_raw();

void clear_below();

std::string readline();

}
