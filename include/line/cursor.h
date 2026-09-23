#pragma once

#include <string>

using namespace std; // TODO: remove

namespace ov4
{

struct T_position
{
    int row;
    int col;
};

// 1-based
inline int cursor_row;
inline int cursor_col;
inline int line_end_pos;
inline int line_begin_pos;

// 0-based index
inline int line_buffer_index;

T_position get_cursor_position();

void print_override(const std::string &s);

void move_cursor(char c);

void cursor_input(char c, std::string &buffer);

int get_cursor_real_pos();

}

