#include "util/prompt.h"

using namespace std;

namespace ov4
{

    bool emit_prompt = true; /* emit prompt (default) */

    void print_prompt()
    {
        if (emit_prompt) 
        {
            char buffer[hostname_size];
            gethostname(buffer, hostname_size);
	        cout 
                << make_color((string)name, ov4::color::bright_green) 
                << " "
                << make_color(getlogin(), ov4::color::bright_blue)
                << make_color("@", ov4::color::bright_blue)
                << make_color(buffer, ov4::color::bright_blue)
                << ":"
                << make_color(get_current_dir(), ov4::color::bright_purple)
                << " " 
                << make_color(get_prompt(), ov4::color::bright_yellow)
                << " " 
                << flush;
	    }
    }

    string get_prompt()
    {
        return (string)((string)(getlogin()) == "root" ? prompt_root : prompt_normal);
    }
}
