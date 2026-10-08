#include "line/cursor.h"

#include <print>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <termios.h>
#include <sys/ioctl.h>

using namespace std;

namespace ov4
{

void print_override(const std::string &s)
{
    // save current cursor position
    print("\x1b[s"); fflush(stdout);

    // move to the "start" of the zone
    // should skip prompt zone
    print("\r"); fflush(stdout);
    print("\x1b[{}C", prompt_pos-1); fflush(stdout);

    // erase rest of the line
    print("\x1b[K"); fflush(stdout);
    
    print("{}", s.substr(buffer_index_display_begin, buffer_index_display_end-buffer_index_display_begin+1)); fflush(stdout);
    //line_end_pos = s.size()+1;

    // restore cursor position
    print("\x1b[u"); fflush(stdout);
}

void move_cursor(char c)
{
    switch (c)
    {
        // case 'A':
        //     print("\x1b[A");
        //     fflush(stdout);
        //     cursor_row--;
        //     break;
        // case 'B':
        //     print("\x1b[B");
        //     fflush(stdout);
        //     cursor_row++;
        //     break;
        case 'C':
            // if (get_cursor_real_pos() < line_end_pos)
            // {
            //     // print("\x1b[C");
            //     // fflush(stdout);
            //     // buffer_index_cursor++;
            //     move_cursor(T_cursor_movement_direction::RIGHT);
            // }
            move_cursor(T_cursor_movement_direction::RIGHT);
            break;
        case 'D':
            // if (get_cursor_real_pos() > prompt_pos)
            // {
            //     // print("\x1b[D");
            //     // fflush(stdout);
            //     // buffer_index_cursor--;
            //     move_cursor(T_cursor_movement_direction::LEFT);
            // }
            move_cursor(T_cursor_movement_direction::LEFT);
            break;
        default:
            return;
    }
}

void cursor_input_char(char c)
{
    // normal input
    if (c != '\b')
    {
        line_buffer.insert(buffer_index_cursor, 1, c);
        // print("\x1b[C"); fflush(stdout); // move cursor to the right
        // buffer_index_cursor++;
        update_line_end_pos();
        int end_possible_buffer_index = window_size_col-prompt_pos-1;
        buffer_index_display_end = 
            (end_possible_buffer_index > (buffer_index_display_end)+1) 
            ? (buffer_index_display_end)+1
            : end_possible_buffer_index;
        move_cursor(T_cursor_movement_direction::RIGHT);
        
        return;
    }

    // BACKSPACE
    line_buffer.erase(buffer_index_cursor, 1);
    // print("\x1b[D"); fflush(stdout); // move cursor to the left
    // buffer_index_cursor--;
    update_line_end_pos();
    // TODO: backspace page
    move_cursor(T_cursor_movement_direction::LEFT);
    
}



void update_window_size() 
{
    winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == -1) 
        exit(-1); // TODO: fix

    window_size_row = ws.ws_row;
    window_size_col = ws.ws_col;
}

void update_line_end_pos()
{
    line_end_pos = prompt_pos + line_buffer.size();
    line_end_pos = (line_end_pos > window_size_col) ? window_size_col : line_end_pos;
}

void move_cursor(T_cursor_movement_direction d)
{
    switch (d)
    {
        case T_cursor_movement_direction::LEFT:
            // if cursor is not at left margin
            if (prompt_pos < (buffer_index_cursor+1))
            {
                buffer_index_cursor--;
                print("\x1b[D"); fflush(stdout); // move cursor to the left
            }
            // margin
            else
            {
                buffer_index_cursor--;
                buffer_index_display_begin--;
                buffer_index_display_end--;
            }
            break;

        case T_cursor_movement_direction::RIGHT:
            // if cursor is not at right margin
            if (line_end_pos > (buffer_index_cursor+1))
            {
                buffer_index_cursor++;
                print("\x1b[C"); fflush(stdout); // move cursor to the right
            }
            // margin
            else
            {
                buffer_index_cursor++;
                buffer_index_display_begin++;
                buffer_index_display_end++;
            }
            break;

        default:
            exit(-1); // TODO: fix
    }
}

}
