#pragma once

#include <string>
#include <unordered_map>

#include "lexer/enum_type.h"

using namespace std;

namespace ov4
{

class T_lexer
{
public:

    // token.h
    class T_token;
    string trim(const string &s, const string &trim_target = " \t");
    bool is_single_symbol(char c);
    void bracket_depth_changer(char c, int &bracket_depth);
    bool token_continue(const string &s, char next);
    void token_push(const string &push_s, int bracket_depth, bool force_text = false);
    int tokenizer(const string &s);

    // ast.h
    class T_ast;
    string final_trim(size_t _begin, size_t _end);
    size_t alloc_ast();
    int get_pivot_order(enum_token_type x);
    bool is_operator(enum_token_type x);
    void parse(size_t cmd_begin, size_t cmd_end, size_t ast_vec);

    // shared
    std::vector<T_ast> ast;
    std::vector<T_token> token;
    size_t ast_index = 0;

};

}
