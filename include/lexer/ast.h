#pragma once

#include <string>

#include "lexer/enum_type.h"
#include "lexer/token.h"
#include "lexer/shared.h"

using namespace std; // remove this

namespace ov4
{

class T_token;

class T_ast
{
public:
    T_ast() = default;

    enum_token_type token_type;
    string command_text;
    size_t left = string::npos;
    size_t right = string::npos;
};

string final_trim(size_t _begin, size_t _end)
{
    string s;
    for (size_t i = _begin; i < _end; i++)
    {
        if (token[i].token_type != TEXT)
            continue;
        s += token[i].text + " ";
    }
    s = s.substr(0, s.size()-1); // eat final space
    return s;
}

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

bool is_operator(enum_token_type x)
{
    switch(x)
    {
        case LOGIC_AND:
        case LOGIC_OR:
        case PIPE:
        case ASYNC:
            return true;
        default:
            return false;
    }
}

// region [begin, end)
void parse(size_t cmd_begin, size_t cmd_end, size_t ast_vec)
{
    int current_depth = 0;
    int current_order = -1;
    int max_depth = -1;
    size_t found_pos = string::npos;

    for (size_t i = cmd_begin; i < cmd_end; i++)
    {
        if (token[i].bracket_depth > max_depth)
            max_depth = token[i].bracket_depth;
    }


    while (found_pos == string::npos)
    {
        // scan from right to left
        for (size_t i = cmd_end - 1; i >= cmd_begin && i < cmd_end; i--)
        {
            // if same depth
            // and higher order number
            if (token[i].bracket_depth == current_depth 
                && is_operator(token[i].token_type) 
                && get_pivot_order(token[i].token_type) > current_order)
            {
                current_order =  get_pivot_order(token[i].token_type);
                found_pos = i;
                println("Found pos: {} order: {} depth: {}", i, current_order, current_depth);
            }
        }
        current_depth++;
        if (current_depth > max_depth)
        {
            break;
        }
    }

    if (found_pos == string::npos)
    {
        println("Smallest unit");
        ast[ast_vec].token_type = TEXT;
        ast[ast_vec].command_text = final_trim(cmd_begin, cmd_end);
        return;
    }

    size_t l = alloc_ast();
    size_t r = alloc_ast();
    ast[ast_vec].left = l;
    ast[ast_vec].right = r;
    ast[ast_vec].token_type = token[found_pos].token_type;

    println("Parse sub: {}, {}, {}", cmd_begin, found_pos, cmd_end);

    parse(cmd_begin, found_pos, l);
    parse(found_pos+1, cmd_end, r);
}

}
