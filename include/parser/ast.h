#pragma once

#include <string>

#include "parser/enum_type.h"
#include "parser/shared.h"

using namespace std; // remove this

namespace ov4
{

class T_ast
{
public:
    T_ast() = default;

    enum_token_type token_type;
    string command_text;
    size_t left;
    size_t right;
};

size_t alloc_ast()
{
    ast.emplace_back();
    return ast_index++;
}

int get_pivot_order(enum_token_type x)
{
    switch(x)
    {
        case ASYNC:
        case SEMICOLON:
            return 1000;
        
        case LOGIC_AND:
        case LOGIC_OR:
            return 500;

        case PIPE:
            return 100;
            
        default:
            return 0;
    }
    return 0;
}

void parse(size_t cmd_begin, size_t cmd_end, size_t ast_vec)
{
    ;
}

}
