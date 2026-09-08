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
    enum_token_type type = TEXT;
};

unordered_map<string, T_property> symbol_property = {
    // non-operator
    {"\'",   {.type = SINGLE_QUOTATION}},
    {"\"",   {.type = SINGLE_QUOTATION}},

    // operator
    {"&",    {.operand_number = 1, .continuable = true,  .pivot_priority = 1000, .type = ASYNC}},
    {"|",    {.operand_number = 1, .continuable = true,  .pivot_priority = 100,  .type = PIPE}},
    {"&&",   {.operand_number = 1, .continuable = true,  .pivot_priority = 1000, .type = LOGIC_AND}},
    {"||",   {.operand_number = 1, .continuable = true,  .pivot_priority = 1000, .type = LOGIC_OR}},
};
    
}
