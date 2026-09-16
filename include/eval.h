#pragma once

#include "all.h"
#include "lexer/lexer_class.h"

namespace ov4
{
int eval_exe(const string &s, bool is_async);
int eval_exe(const string &s, bool is_async, const T_lexer *lexer_instance, size_t i);
int eval_tree_cd(size_t i, const T_lexer &lexer_instance, bool inside_subshell);
void eval(char *cmdline);

}
