/*
 * test line editor
   $ g++ \
   module-test/line_test.cc \
   -std=c++26 -Iinclude -Ithird_party/magic_enum/include \
   -O0 -ggdb3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_ASSERTIONS \
   -o module-test/line_test.out
 *
 */


#include <iostream>
#include <print>

#include "line/mode.h"

using namespace std;
using namespace ov4;

int main()
{
    enable_raw();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (c == 'q') break;
        if (iscntrl(c))
            print("control: {}\r\n", static_cast<unsigned char>(c));
        else
            print("entered: {}\r\n", c);
    }
    return 0;
}
