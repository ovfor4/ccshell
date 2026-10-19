#include "util/prompt.h"

using namespace std;

namespace ov4
{

    bool emit_prompt = true; /* emit prompt (default) */

    int print_prompt()
    {
        if (emit_prompt) 
        {
            char hostname[MAXLINE];
            gethostname(hostname, MAXLINE);

            string _getlogin = safe_str(getlogin());
            string _gethostname = safe_str(hostname);
            string _get_current_dir = get_current_dir();
            string _get_prompt = get_prompt_symbol();
            string _at_symbol = "@";
            string _space = " ";
            string _semicolon = ":";

	        cout 
                << make_color((string)name, ov4::color::bright_green) 
                << _space
                << make_color(_getlogin , ov4::color::bright_blue)
                << make_color(_at_symbol, ov4::color::bright_blue)
                << make_color(_gethostname, ov4::color::bright_blue)
                << _semicolon
                << make_color(_get_current_dir, ov4::color::bright_purple)
                << _space
                << make_color(_get_prompt, ov4::color::bright_yellow)
                << _space
                << flush;

            return 
                name.size() 
                + _space.size()
                + _getlogin.size() 
                + _at_symbol.size() 
                + _gethostname.size() 
                + _semicolon.size()
                + _get_current_dir.size()
                + _space.size()
                + _get_prompt.size() 
                + _space.size();
	    }
        return 0;
    }

    string get_prompt_symbol()
    {
        return (string)(safe_str(getlogin()) == "root" ? prompt_root : prompt_normal);
    }
}
