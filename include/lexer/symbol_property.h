#pragma once

#include <unordered_map>
#include <string>

#include "lexer/enum_type.h"

using namespace std;

namespace ov4
{

class T_property
{
public:
    bool continuable = true;

    // -1 means it's NOT operator
    // any positive values mean it's operator
    int pivot_priority = -1;

    // require: MUST
    // accept: optional, but if it doesn't accept, but there is indeed a child, then error
    child_existence_type left;
    child_existence_type right;

    enum_token_type enum_type = TEXT;
};

inline const unordered_map<string, T_property> symbol_property = {
    // non-operator
    {"\'",   {.enum_type = SINGLE_QUOTATION}},
    {"\"",   {.enum_type = SINGLE_QUOTATION}},

    // bracket
    {"(",    {.continuable = false, .enum_type = LEFT_BRACKET}},
    {")",    {.continuable = false, .enum_type = RIGHT_BRACKET}},

    // operator
    {"&",    {.continuable = true, .pivot_priority = 2000, .left = child_existence_type::EXIST,     .right = child_existence_type::NOT_EXIST, .enum_type = ASYNC}},
    {"|",    {.continuable = true, .pivot_priority = 100,  .left = child_existence_type::EXIST,     .right = child_existence_type::EXIST,     .enum_type = PIPE}},
    {"&&",   {.continuable = true, .pivot_priority = 1000, .left = child_existence_type::EXIST,     .right = child_existence_type::EXIST,     .enum_type = LOGIC_AND}},
    {"||",   {.continuable = true, .pivot_priority = 1000, .left = child_existence_type::EXIST,     .right = child_existence_type::EXIST,     .enum_type = LOGIC_OR}},

};

string symbol_enum2string(enum_token_type e);

bool symbol_contains_enum_type(enum_token_type e);
    
}
