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

void print_override(const std::string &s)
{

    print("\x1b[s"); fflush(stdout);
    print("\r"); fflush(stdout);

    // move to the "start" of the zone
    // should skip prompt zone
    print("\x1b[{}C", line_begin_pos-1); fflush(stdout);

    // erase rest of it
    print("\x1b[K"); fflush(stdout);
    
    print("{}", s);
    fflush(stdout);
    //line_end_pos = s.size()+1;
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
            if (get_cursor_real_pos() < line_end_pos)
            {
                print("\x1b[C");
                fflush(stdout);
                line_buffer_index++;
            }
            break;
        case 'D':
            if (get_cursor_real_pos() > line_begin_pos)
            {
                print("\x1b[D");
                fflush(stdout);
                line_buffer_index--;
            }
            break;
        default:
            return;
    }
}

void cursor_input(char c, std::string &buffer)
{
    // normal input
    if (c != '\b')
    {
        buffer.insert(line_buffer_index, 1, c);
        print("\x1b[C"); fflush(stdout);
        line_buffer_index++;
        buffer_len = buffer.size();
        update_line_end_pos();
        return;
    }

    // BACKSPACE
    buffer.erase(line_buffer_index, 1);
    print("\x1b[D"); fflush(stdout);
    line_buffer_index--;
    buffer_len = buffer.size();
    update_line_end_pos();
}

int get_cursor_real_pos()
{
    return line_begin_pos + line_buffer_index;
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
    line_end_pos = line_begin_pos + buffer_len;
    line_end_pos = (line_end_pos > window_size_col) ? window_size_col : line_end_pos;
}

}
