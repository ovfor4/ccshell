#pragma once

#include "line/type.h"

namespace ov4
{

int get_terminal_pos_cursor();
int get_buffer_index_display_begin();
int get_buffer_index_display_end();
int get_terminal_pos_line_end();

//void update_line_end_pos();

T_position get_cursor_position();

}