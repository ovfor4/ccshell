#include "lexer/parseline.h"

#include <vector>

#include "lexer/enum_type.h"
#include "util/string.h"

using namespace std;

namespace ov4
{

char **parseline(const std::string &s)
{
    guard_type guard = GUARD_OFF;
    vector<string> v;
    size_t len = s.size();
    string prev_str = "";
    char c;

    for (size_t i = 0; i < len; i++)
    {

        c = s[i];
        if (c == ' ' && guard == GUARD_OFF) continue;
        
        if (c == '\'' || c == '\"')
        {

            // turn on GUARD
            if (guard == GUARD_OFF)
            {
                guard = (c == '\'') ? GUARD_STRONG : GUARD_WEAK;
                if (i+1 == len)
                {
                    v.push_back(prev_str);
                    prev_str = "";
                }
                continue;
            }
            // turn off
            else if (guard == GUARD_STRONG && c == '\'')
            {
                guard = GUARD_OFF;
                if (i+1 == len)
                {
                    v.push_back(prev_str);
                    prev_str = "";
                }
                continue;
            }
            // turn off
            else if (guard == GUARD_WEAK && c == '\"')
            {
                guard = GUARD_OFF;
                if (i+1 == len)
                {
                    v.push_back(prev_str);
                    prev_str = "";
                }
                continue;
            } 
            // if this (single/double) quotation mark is guarded
            // it should be saved
        }

        prev_str += c;

        if ((i+1 >= len) || (s[i+1] == ' ' && guard == GUARD_OFF))
        {
            v.push_back(prev_str);
            prev_str = "";
            continue;
        }
    }

    return string2argv_alloc(v);
}

}
