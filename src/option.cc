#include "option.h"

#include <string>
#include <iostream>

#include "global.h"
#include "util/io.h"

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
    
}

