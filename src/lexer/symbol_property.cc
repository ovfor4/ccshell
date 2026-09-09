#include "lexer/symbol_property.h"

#include <string>

#include "lexer/enum_type.h"

using namespace std;

namespace ov4
{

string symbol_enum2string(enum_token_type e)
{
    for (auto &c : symbol_property)
    {
        // pair<string, T_property>
        
        if (c.second.enum_type == e)
            return c.first;
    }
    return "";
}

bool symbol_contains_enum_type(enum_token_type e)
{
    return (symbol_enum2string(e) == "") ? false : true;
}

}
