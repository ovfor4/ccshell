#include "line/getter.h"

#include <cstdio>
#include <string>
#include <termios.h>
#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>
#include <cmath>

#include "line/cursor.h"


using namespace std;

namespace ov4
{

int get_terminal_pos_cursor()
{
    return prompt_pos + buffer_index_cursor;
}

int get_buffer_index_display_begin()
{
    return _buffer_index_display_begin;
}

int get_buffer_index_display_end()
{
    int max_possible_end = get_buffer_index_display_begin() + line_buffer.size() - 1;
    int window_possible_end = get_buffer_index_display_begin() + (window_size_col - prompt_pos) -1;
    return min(max_possible_end, window_possible_end);
}

void update_line_end_pos()
{
    line_end_pos = prompt_pos + line_buffer.size();
    line_end_pos = (line_end_pos > window_size_col) ? window_size_col : line_end_pos;
}


T_position get_cursor_position()
{
    char c;
    string buffer;
    T_position pos;
    int row, col;
    tcflush(STDIN_FILENO, TCIFLUSH);
    cout << "\x1b[6n" << flush;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (c == 'R') break;
        buffer += c;
    }
    if (buffer.size() <= 2 || buffer[0] != '\x1b' || buffer[1] != '[')
    {
        pos.row = -1;
        pos.col = -1;
        return pos;
    }
    if ((sscanf(buffer.c_str(), "\x1b[%d;%d", &row, &col)) != 2)
    {
        pos.row = -1;
        pos.col = -1;
        return pos;
    }
    pos.row = row;
    pos.col = col;
    // print("row {}, col {}\r\n", row, col);
    // fflush(stdout);
    return pos;
} 
    
}