#include "lexer/ast.h"

#include <string>
#include <iostream>
#include <print>

#include "lexer/lexer_class.h"
#include "lexer/enum_type.h"
#include "lexer/token.h"
#include "lexer/symbol_property.h"
#include "util/io.h"

using namespace std;

namespace ov4
{

size_t T_lexer::alloc_ast()
{
    ast.emplace_back();
    return ast_index++;
}

string T_lexer::final_trim(size_t _begin, size_t _end)
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

int T_lexer::get_pivot_order(enum_token_type x)
{
    string s = symbol_enum2string(x);
    if (s != "")
    {
        return symbol_property[s].pivot_priority;
    }
    // not found
    return -1;
}

bool T_lexer::is_operator(enum_token_type x)
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
void T_lexer::parse(size_t cmd_begin, size_t cmd_end, size_t ast_vec)
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
                loggerln("Found pos: {} order: {} depth: {}", i, current_order, current_depth);
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
        loggerln("Smallest unit");
        ast[ast_vec].token_type = TEXT;
        ast[ast_vec].command_text = final_trim(cmd_begin, cmd_end);
        return;
    }

    // left child

    string token_str = symbol_enum2string(token[found_pos].token_type);

    // miss left
    if (symbol_property[token_str].left == EXIST && cmd_begin == found_pos)
    {
        loggerln("parse: missing left child");
        exit(-1); // TODO
    }

    // miss right
    if (symbol_property[token_str].right == EXIST && cmd_end-1 == found_pos)
    {
        loggerln("parse: missing right child");
        exit(-1); // TODO
    }

    // should not exist left
    if (symbol_property[token_str].left == NOT_EXIST && cmd_begin != found_pos)
    {
        loggerln("parse: left child should NOT exist");
        exit(-1); // TODO
    }

    // should not exist right
    if (symbol_property[token_str].right == NOT_EXIST && cmd_end-1 != found_pos)
    {
        loggerln("parse: right child should NOT exist");
        exit(-1); // TODO
    }

    size_t l = alloc_ast();
    size_t r = alloc_ast();
    ast[ast_vec].left = l;
    ast[ast_vec].right = r;
    ast[ast_vec].token_type = token[found_pos].token_type;

    loggerln("Parse sub: {}, {}, {}", cmd_begin, found_pos, cmd_end);

    parse(cmd_begin, found_pos, l);
    parse(found_pos+1, cmd_end, r);
}

}
