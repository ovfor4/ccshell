#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <string>

#include "magic_enum/magic_enum.hpp"

#include "error.h"
#include "lang_analysis/lexer_class.h"
#include "lang_analysis/ast.h"
#include "lang_analysis/token.h"
#include "handler.h"

using namespace std;

namespace ov4
{
    
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *raw_input) 
{
    string s = raw_input;
    T_lexer lexer_instance;

    try
    {
        lexer_instance.tokenizer(s);
        lexer_instance.parse(0, lexer_instance.token.size(), lexer_instance.alloc_ast());
    }
    catch(const T_error &e)
    {
        cerr << "Error: " << magic_enum::enum_name(e.code) << endl;
        cerr << "what: " << e.what() << endl;
        return;
    }

    eval_tree_cd(0, lexer_instance, false);
    return;
}

}
