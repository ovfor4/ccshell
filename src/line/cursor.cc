#include "line/cursor.h"

#include <print>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <termios.h>
#include <sys/ioctl.h>

#include "line/getter.h"
#include "history.h"
#include "line/mode.h"

using namespace std;

namespace ov4
{

void print_override(const std::string &s)
{
    // save current cursor position
    // print("\x1b[s"); fflush(stdout);
    // WTF
    // ESC 7     Save Cursor (DECSC), VT100.
    print("\x1b" "7"); fflush(stdout);

    cursor_skip_prompt();

    // erase rest of the line
    print("\x1b[K"); fflush(stdout);
    
    print("{}", s.substr(get_buffer_index_display_begin(), 
        get_buffer_index_display_end() - get_buffer_index_display_begin() + 1)); fflush(stdout);
    //line_end_pos = s.size()+1;

    // restore cursor position
    // ESC 8     Restore Cursor (DECRC), VT100.
    print("\x1b" "8"); fflush(stdout);
}

void move_cursor(char c)
{
    switch (c)
    {
        case 'A':
            switch_history(T_cursor_movement_direction::UP);
            break;
        case 'B':
            switch_history(T_cursor_movement_direction::DOWN);
            break;
        case 'C':
            move_cursor(T_cursor_movement_direction::RIGHT);
            break;
        case 'D':
            move_cursor(T_cursor_movement_direction::LEFT);
            break;
        default:
            return;
    }
}

void cursor_input_char(char c)
{
    // if receives any input
    // it should move to the latest history
    history_index = get_last_index();
    // normal input
    if (c != '\b')
    {
        line_buffer.insert(buffer_index_cursor, 1, c);
        move_cursor(T_cursor_movement_direction::RIGHT);
        
        return;
    }

    // BACKSPACE
    line_buffer.erase(buffer_index_cursor, 1);
    // TODO: backspace page
    move_cursor(T_cursor_movement_direction::LEFT);
    
}



void update_window_size() 
{
    winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == -1) 
        editor_exit(-1); // TODO: fix

    window_size_row = ws.ws_row;
    window_size_col = ws.ws_col;
}

void move_cursor(T_cursor_movement_direction d)
{
    switch (d)
    {
        case T_cursor_movement_direction::LEFT:
            // if cursor is not at left margin
            if (prompt_pos+1 < get_terminal_pos_cursor())
            {
                buffer_index_cursor--;
                print("\x1b[D"); fflush(stdout); // move cursor to the left
            }
            // margin
            else
            {
                // already at the beginning of buffer
                if (buffer_index_cursor == 0)
                {
                    print("\a"); fflush(stdout);
                    break;
                }

                buffer_index_cursor--;
                _buffer_index_display_begin--;
                print_override(line_buffer);
            }
            break;

        case T_cursor_movement_direction::RIGHT:
            // if cursor is not at right margin
            if (window_size_col > get_terminal_pos_cursor())
            {
                // already at the end of buffer
                if (buffer_index_cursor == line_buffer.size())
                {
                    print("\a"); fflush(stdout);
                    break;
                }

                buffer_index_cursor++;
                print("\x1b[C"); fflush(stdout); // move cursor to the right
            }
            // margin
            else
            {
                // already at the end of buffer
                if (buffer_index_cursor == line_buffer.size())
                {
                    print("\a"); fflush(stdout);
                    break;
                }

                buffer_index_cursor++;
                _buffer_index_display_begin++;
                print_override(line_buffer);
            }
            break;

        default:
            editor_exit(-1); // TODO: fix
    }
}

void switch_history(T_cursor_movement_direction d)
{
    switch (d)
    {
        case T_cursor_movement_direction::UP:
            if (history_index == 0)
            {
                print("\a"); fflush(stdout);
            }
            else
            {
                history_index--;
                reset_editor();
                line_buffer = history_vec[history_index];
                print_override(line_buffer);
            }
            break;

        case T_cursor_movement_direction::DOWN:
            if (history_index == get_last_index())
            {
                print("\a"); fflush(stdout);
            }
            else
            {
                history_index++;
                reset_editor();
                line_buffer = history_vec[history_index];
                print_override(line_buffer);
            }
            break;

        default:
            editor_exit(-1); // TODO: fix
    }
}

void reset_editor()
{
    line_buffer = "";
    buffer_index_cursor = 0;
    _buffer_index_display_begin = 0;
    cursor_skip_prompt();
}

void cursor_skip_prompt()
{
    // move to the "start" of the zone
    // should skip prompt zone
    print("\r\x1b[{}C", prompt_pos-1); fflush(stdout);
}

}
