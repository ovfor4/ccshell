/*
 * test line editor
   $ g++ \
   module-test/line_test.cc \
   src/line/mode.cc \
   src/line/cursor.cc \
   src/line/getter.cc \
   src/line/input.cc \
   src/history.cc \
   src/path.cc \
   src/util/string.cc \
   src/signal/misc.cc \
   src/signal/sigwinch_handler.cc \
   -std=c++26 -Iinclude -Ithird_party/magic_enum/include \
   -O0 -ggdb3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_ASSERTIONS \
   -o module-test/line_test.out\
   \
   src/option.cc \
   src/util/io.cc \
   src/signal/sigint_handler.cc \
   src/signal/sigtstp_handler.cc \
   src/signal/sigchld_handler.cc \
   src/signal/sigquit_handler.cc \
   src/job.cc \
   src/util/prompt.cc \
   src/util/color.cc
 *
 */


#include <iostream>
#include <print>
#include <unistd.h>

#include "line/mode.h"
#include "line/cursor.h"
#include "line/getter.h"
#include "line/type.h"
#include "line/input.h"
#include "history.h"
#include "path.h"

using namespace std;
using namespace ov4;

int main()
{
    load_history();
    while (true)
    {
        print("shell $ ");
        enable_raw();
        T_position pos = get_cursor_position();
        cursor_row = pos.row;
        cursor_col = pos.col;
        prompt_pos = cursor_col;
        string s = readline();
        disable_raw();
        cout << "\n\n\n\n" << endl;
        cout << s << endl;
        if (s == "quit") break;
    }
    save_history();
    return 0;
}
