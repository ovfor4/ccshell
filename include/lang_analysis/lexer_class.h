#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "lang_analysis/enum_type.h"

namespace ov4
{

class T_lexer
{
public:

    // token.h
    class T_token;
    std::string trim(const std::string &s, const std::string &trim_target = " \t");
    bool is_single_symbol(char c);
    void bracket_depth_changer(char c, int &bracket_depth);
    bool token_continue(const std::string &s, char next);
    void token_push(const std::string &push_s, int bracket_depth, bool force_text = false);
    int tokenizer(const std::string &s);

    // ast.h
    class T_ast;
    std::string final_trim(std::size_t _begin, std::size_t _end);
    std::size_t alloc_ast();
    int get_pivot_order(enum_token_type x);
    bool is_operator(enum_token_type x);
    void parse(std::size_t cmd_begin, std::size_t cmd_end, std::size_t ast_vec);

    // shared
    std::vector<T_ast> ast;
    std::vector<T_token> token;
    std::size_t ast_index = 0;

};

}
