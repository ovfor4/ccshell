#pragma once

#include <string>

#include "lexer/lexer_class.h"
#include "lexer/enum_type.h"
#include "lexer/token.h"

using namespace std; // remove this

namespace ov4
{

class T_lexer::T_ast
{
public:
    T_ast() = default;

    enum_token_type token_type;
    string command_text;
    size_t left = string::npos;
    size_t right = string::npos;
};

}
