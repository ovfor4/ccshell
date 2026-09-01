#include "util/prompt.h"

namespace ov4
{
    using namespace std;

    bool emit_prompt = true; /* emit prompt (default) */

    void print_prompt()
    {
        if (emit_prompt) 
        {
	        cout << name << " : " << get_current_dir() << " " << prompt << " " << flush;
	    }
    }
}
