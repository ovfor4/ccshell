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

enum class T_cursor_movement_direction
{
    LEFT,
    RIGHT,
};

// 1-based
inline int cursor_row;
inline int cursor_col;
inline int line_end_pos;
inline int line_begin_pos;

inline int window_size_row;
inline int window_size_col;

// 0-based index
inline int line_buffer_index_cursor;
// index [begin, end] is displayed
// NB: CLOSE interval
inline int line_buffer_index_begin;
inline int line_buffer_index_end;

inline int buffer_len;

inline std::string line_buffer;

T_position get_cursor_position();

void print_override(const std::string &s);

void move_cursor(char c);

void cursor_input_char(char c);

int get_cursor_real_pos();

void update_window_size();

void update_line_end_pos();

void move_cursor(T_cursor_movement_direction d);

}

