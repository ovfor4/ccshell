#include "line/cursor.h"

#include <print>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <termios.h>

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
    print("row {}, col {}\r\n", row, col);
    fflush(stdout);
    return pos;
} 

void print_override(const std::string &s)
{
    print("\x1b[s"); fflush(stdout);
    print("\r");
    fflush(stdout);
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
            if (cursor_col < line_end_pos)
            {
                print("\x1b[C");
                fflush(stdout);
                cursor_col++;
            }
            break;
        case 'D':
            if (cursor_col > 1)
            {
                print("\x1b[D");
                fflush(stdout);
                cursor_col--;
            }
            break;
        default:
            return;
    }
}

void cursor_input(char c, std::string &buffer)
{
    if (c != '\b')
    {
        buffer.insert(cursor_col-1, 1, c);
        print("\x1b[C"); fflush(stdout);
        cursor_col++;
        line_end_pos = buffer.size()+1;
        return;
    }

    // BACKSPACE
    buffer.erase(cursor_col-1, 1);
    print("\x1b[D"); fflush(stdout);
    cursor_col--;
    line_end_pos = buffer.size()+1;
}

}
