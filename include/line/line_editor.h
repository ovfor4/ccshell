#pragma once

#include <termios.h>
#include <string>

#include "line/type.h"

namespace ov4
{

// mode
inline termios attr;
inline bool raw_enabled = false;

void disable_raw();
void enable_raw();
void editor_exit(int code);

class T_line_editor
{
public:
    // cursor

    // 1-based
    int cursor_row = 1;
    int cursor_col = 1;
    int prompt_pos = 1;
    int window_size_row = 24;
    int window_size_col = 80;

    // 0-based index
    int buffer_index_cursor = 0;
    // index [begin, end] is displayed
    // NB: CLOSE interval
    int _buffer_index_display_begin = 0;
    //int buffer_index_display_end;

    std::string line_buffer;

    void print_override(const std::string &s);
    void move_cursor(char c);
    void cursor_input_char(char c);
    void update_window_size();
    void move_cursor(T_cursor_movement_direction d);
    void switch_history(T_cursor_movement_direction d);
    void reset_editor();
    void cursor_skip_prompt();
    void check_window_update();


    // getter
    bool windows_size_changed = false;

    int get_terminal_pos_cursor();
    int get_buffer_index_display_begin();
    int get_buffer_index_display_end();
    int get_terminal_pos_line_end();

    //void update_line_end_pos();

    T_position get_cursor_position();


    // input
    void clear_below();
    std::string readline();
};

}