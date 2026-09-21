#include "line/mode.h"

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <print>

using namespace std;

namespace ov4
{

void disable_raw()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr);
}

void enable_raw()
{
    termios attr_raw;
    tcgetattr(STDIN_FILENO, &attr);
    tcgetattr(STDIN_FILENO, &attr_raw);

    // LOCAL

    // disable
    // echo
    // canonical
    // signal
    // disable post-input processing, aka control-V
    attr_raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    // INPUT

    // disable 
    // software flow control, aka control-S/Q
    // \n and \r conversion
    attr_raw.c_iflag &= ~(IXON | ICRNL);

    // OUTPUT

    // disable
    // post-output processing
    attr_raw.c_oflag &= ~(OPOST);

    // misc
    attr_raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP);
    attr_raw.c_cflag |= ~(CS8);

    atexit(disable_raw);

    // TCSAFLUSH
    // the change shall occur after all output written to fildes is transmitted
    // and all input so far received but not read shall be discarded before the change is made
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr_raw);
}

void clear_below()
{
    print("\x1b[0J");
}

string readline()
{
    string buffer;
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (c == 'Q') break;
        else if (c == 'C') 
        {
            clear_below();
        }
        else if (c == 'R')
        {
            get_cursor_position();
        }
        else if (iscntrl(c))
        {
            print("control: {}\r\n", static_cast<unsigned char>(c));
            fflush(stdout);
            if (c == 13) // \n
            {
                //print("line: {}\r\n", buffer);
                return buffer;
            }
        }
        else
        {
            print("{}", c);
            buffer += c;
            fflush(stdout);
        }
    }
    return buffer;
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
    print("row {}, col {}\r\n", row, col);
    fflush(stdout);
    return pos;
} 

}
