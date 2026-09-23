/*
 * test line editor
   $ g++ \
   module-test/line_test.cc \
   src/line/mode.cc \
   src/line/cursor.cc \
   -std=c++26 -Iinclude -Ithird_party/magic_enum/include \
   -O0 -ggdb3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_ASSERTIONS \
   -o module-test/line_test.out
 *
 */


#include <iostream>
#include <print>
#include <unistd.h>

#include "line/mode.h"
#include "line/cursor.h"

using namespace std;
using namespace ov4;

int main()
{
    print("shell $ ");
    enable_raw();
    T_position pos = get_cursor_position();
    cursor_row = pos.row;
    cursor_col = pos.col;
    string s = readline();
    disable_raw();
    cout << "\n\n\n\n" << endl;
    cout << s << endl;
    return 0;
}
