#pragma once

#include <string>

#include "lang_analysis/lexer_class.h"
#include "lang_analysis/enum_type.h"
#include "lang_analysis/symbol_property.h"

namespace ov4
{

class T_lexer::T_token
{
public:
    T_token() = default;

    enum_token_type token_type;
    std::string text;
    guard_type guard = GUARD_OFF;
    int bracket_depth = 0;
};

}
