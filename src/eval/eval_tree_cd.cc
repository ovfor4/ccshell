#include "eval.h"

#include <string>

#include "error.h"
#include "lang_analysis/lexer_class.h"
#include "lang_analysis/ast.h"
#include "lang_analysis/token.h"
#include "lang_analysis/enum_type.h"
#include "lang_analysis/symbol_property.h"
#include "util/io.h"

using namespace std;

namespace ov4
{

int eval_tree_cd(size_t i, const T_lexer &lexer_instance, bool inside_subshell)
{
    // TODO: async

    int ret = -1;

    println("eval_tree_cd: processing {} subshell {}", i, (lexer_instance.ast[i].subshell ? "TRUE" : "FALSE"));

    // if it's a subshell, and current AST's subshell status is not subshell
    // (subshell should be forkerd already, then set inside_subshell = true)
    // fork itself
    if (lexer_instance.ast[i].subshell && !inside_subshell)
    {
        eval_exe("", false, &lexer_instance, i);
        return 0;
    }

    if (lexer_instance.ast[i].token_type == TEXT)
    {
        ret = eval_exe(lexer_instance.ast[i].command_text, false);
        return ret;
    }

    if (lexer_instance.ast[i].left != string::npos)
    {
        ret = eval_tree_cd(lexer_instance.ast[i].left, lexer_instance, false);
    }

    if (lexer_instance.ast[i].right != string::npos)
    {
        if (lexer_instance.ast[i].token_type == LOGIC_AND && ret != 0) return ret;
        if (lexer_instance.ast[i].token_type == LOGIC_OR && ret == 0) return ret;
        
        ret = eval_tree_cd(lexer_instance.ast[i].right, lexer_instance, false);
    }
    return ret;
}

}