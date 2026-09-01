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
                << name 
                << " "
                << getlogin()
                << "@"
                << buffer
                << ":"
                << get_current_dir() 
                << " " 
                << get_prompt() 
                << " " 
                << flush;
	    }
    }

    string get_prompt()
    {
        return (string)((string)(getlogin()) == "root" ? prompt_root : prompt_normal);
    }
}
