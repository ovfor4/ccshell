#pragma once

#include <string>

#include "lang_analysis/lexer_class.h"
#include "lang_analysis/enum_type.h"
#include "lang_analysis/token.h"

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

    // if this is marked as `subshell`
    // it will fork a subshell
    // then the subshell will handler the rest
    bool subshell;
};

}
