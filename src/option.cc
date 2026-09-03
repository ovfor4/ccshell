#include "option.h"

#include <string>
#include <iostream>

#include "global.h"
#include "util/io.h"
#include "util/prompt.h"

using namespace std;

namespace ov4::option
{
    
void v(string s)
{
    if (s == "" || s == "true" || s == "1")
    {
        ov4::verbose = true;
        LOG << "verbose = true" << endl;
        return;
    }
    if (s == "false" || s == "0")
    {
        ov4::verbose = false;
        LOG << "verbose = flase" << endl;
        return;
    }
    LOG << "-v: invalid value" << endl;
}

void p(string s)
{
    if (s == "" || s == "true" || s == "1")
    {
        ov4::emit_prompt = true;
        LOG << "emit_prompt = true" << endl;
        return;
    }
    if (s == "false" || s == "0")
    {
        ov4::emit_prompt = false;
        LOG << "emit_prompt = flase" << endl;
        return;
    }
    LOG << "-p: invalid value" << endl;
}


/*
 * usage - print a help message
 */
void h([[maybe_unused]] string s)
{
    cout << "\
Usage: ccshell [-hvp] \n\
    -h                  print this message \n\
    -v(=<bool>)         print additional diagnostic information \n\
    -p(=<bool>)         do not emit a command prompt \
" << endl;
    exit(1);
}

}
