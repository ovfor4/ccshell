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
    int operand_number = 0;
    bool continuable = true;
    int pivot_priority = -1;

    // require: MUST
    // accept: optional, but if it doesn't accept, but there is indeed a child, then error
    child_existance_type left;
    child_existance_type right;

    enum_token_type enum_type = TEXT;
};

unordered_map<string, T_property> symbol_property = {
    // non-operator
    {"\'",   {.enum_type = SINGLE_QUOTATION}},
    {"\"",   {.enum_type = SINGLE_QUOTATION}},

    // bracket
    {"(",    {.continuable = false, .enum_type = LEFT_BRACKET}},
    {")",    {.continuable = false, .enum_type = RIGHT_BRACKET}},

    // operator
    {"&",    {.operand_number = 1, .continuable = true, .pivot_priority = 1000, .left = EXIST,     .right = NOT_EXIST, .enum_type = ASYNC}},
    {"|",    {.operand_number = 1, .continuable = true, .pivot_priority = 100,  .left = EXIST,     .right = EXIST,     .enum_type = PIPE}},
    {"&&",   {.operand_number = 1, .continuable = true, .pivot_priority = 1000, .left = EXIST,     .right = EXIST,     .enum_type = LOGIC_AND}},
    {"||",   {.operand_number = 1, .continuable = true, .pivot_priority = 1000, .left = EXIST,     .right = EXIST,     .enum_type = LOGIC_OR}},

};

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
