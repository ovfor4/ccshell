#pragma once

#include "all.h"
#include "lang_analysis/lexer_class.h"

namespace ov4
{

struct T_pipe
{
    enum class T_pipe_type
    {
        NOT_PIPE,
        WRITE_PIPE,
        READ_PIPE,
    };
    T_pipe_type pipe_type =T_pipe_type::NOT_PIPE;
    int fd = -1; 
};
inline T_pipe T_pipe_not_pipe;

int eval_exe(const std::string &s, bool is_async);
int eval_exe(const std::string &s, bool is_async, const T_lexer *lexer_instance, size_t i, const T_pipe &pi = T_pipe_not_pipe);
int eval_tree_cd(size_t i, const T_lexer &lexer_instance, bool inside_subshell, T_pipe pi = T_pipe_not_pipe);
void eval(char *cmdline);
void eval_err(int err);

}
