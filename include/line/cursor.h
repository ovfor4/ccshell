#pragma once

#include <string>

#include "line/type.h"

using namespace std; // TODO: remove

namespace ov4
{

// 1-based
inline int cursor_row;
inline int cursor_col;
//inline int line_end_pos;

inline int prompt_pos;

inline int window_size_row;
inline int window_size_col;

// 0-based index
inline int buffer_index_cursor;
// index [begin, end] is displayed
// NB: CLOSE interval
inline int _buffer_index_display_begin;
//inline int buffer_index_display_end;

inline std::string line_buffer;

void print_override(const std::string &s);

void move_cursor(char c);

void cursor_input_char(char c);

void update_window_size();

void move_cursor(T_cursor_movement_direction d);

}

