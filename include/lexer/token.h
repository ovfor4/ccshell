#pragma once

#include <string>

#include "lexer/lexer_class.h"
#include "lexer/enum_type.h"
#include "lexer/symbol_property.h"

using namespace std; // remove this

namespace ov4
{

class T_lexer::T_token
{
public:
    T_token() = default;

    enum_token_type token_type;
    string text;
    guard_type guard = GUARD_OFF;
    int bracket_depth = 0;
};

}
