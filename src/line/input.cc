#include "line/line_editor.h"

#include <print>
#include <string>
#include <unistd.h>

#include "history.h"
#include "util/non_block_input.h"
#include "pipe.h"

using namespace std;

namespace ov4
{

void T_line_editor::clear_below()
{
    print("\x1b[0J");
}

string T_line_editor::readline(int prompt_len)
{
    prompt_pos = prompt_len+1;
    update_window_size();
    history_vec.push_back("");
    history_index = get_last_index();
    reset_editor();

    char c; int src;
    while (non_block_read_char(&c, &src) == 1)
    {
        // if signal'd
        if (src == self_pipe_fd[0])
        {
            check_window_update();
            continue;
        }
        if (iscntrl(c))
        {
            //print("control: {}\r\n", static_cast<unsigned char>(c));
            //fflush(stdout);
            if (c == 0x0A || c == 0x0D) // \n OR \r
            {
                //print("line: {}\r\n", line_buffer);
                if (line_buffer.empty())
                {
                    history_index--;
                    history_vec.pop_back();
                }
                else
                {
                    history_vec[get_last_index()] = line_buffer;
                }
                return line_buffer;
            }

            else if (c == 0x03) // control C
            {
                history_index--;
                history_vec.pop_back();
                return "";
            }

            // ESC sequence
            else if (c == '\x1b')
            {
                char seq[10];
                // eat [
                if (read(STDIN_FILENO, seq + 0, 1) != 1)
                    continue;

                if (read(STDIN_FILENO, seq + 1, 1) != 1)
                    continue;

                move_cursor(seq[1]);
            }

            // backspace
            else if (c == '\b' || c == '\x7f')
            {
                cursor_input_char(c);
                print_override(line_buffer);
            }
        }

        // normal char
        else
        {
            cursor_input_char(c);
            print_override(line_buffer);
        }
    }

    if (line_buffer.empty())
    {
        history_index--;
        history_vec.pop_back();
    }
    else
    {
        history_vec[get_last_index()] = line_buffer;
    }
    return line_buffer;
}

}