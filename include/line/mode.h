#pragma once

#include <termios.h>
#include <string>

using namespace std; // TODO: remove

namespace ov4
{

struct T_position
{
    int row;
    int col;
};

inline termios attr;
inline int cursor_row;
inline int cursor_col;

void disable_raw();
void enable_raw();

void clear_below();

std::string readline();

T_position get_cursor_position();

void print_override(const std::string &s);

}
