#include "line/mode.h"

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <print>

#include "line/cursor.h"
#include "line/getter.h"
#include "history.h"

using namespace std;

namespace ov4
{

void disable_raw()
{
    raw_enabled = false;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr);
}

void enable_raw()
{
    if (raw_enabled) return;
     
    raw_enabled = true;
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
    attr_raw.c_cflag |= (CS8);

    atexit(disable_raw);

    // TCSAFLUSH
    // the change shall occur after all output written to fildes is transmitted
    // and all input so far received but not read shall be discarded before the change is made
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr_raw);
}

void editor_exit(int code)
{
    disable_raw();
    exit(code);
}


}
