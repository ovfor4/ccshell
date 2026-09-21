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
#include "lang_analysis/enum_type.h"
#include "lang_analysis/symbol_property.h"
#include "lang_analysis/parseline.h"
#include "handler.h"

using namespace std;

extern char **environ;

namespace ov4
{

int eval_exe(const string &s, bool is_async)
{
    return eval_exe(s, is_async, nullptr, string::npos);
}

int eval_exe(const string &s, bool is_async, const T_lexer *lexer_instance, size_t i)
{
    loggerln("eval_exe: receive {}", s);

    bool task_type_is_subshell = false;
    const char *raw_input = nullptr;
    char **argv = nullptr;
    const char *unified_cmd_c = nullptr;
    string unified_cmd, s_wrap_eliminated;

    // subshell type
    if (lexer_instance != nullptr)
    {
        task_type_is_subshell = true;
        unified_cmd  = "SUBSHELL";
        raw_input = unified_cmd.c_str();
    } 
    // command type
    else
    {
        task_type_is_subshell = false;
        s_wrap_eliminated = s.substr(0, s.find('\n')); // eliminate \n
        raw_input = s_wrap_eliminated.c_str();
        
        // NB: remember to delete
        argv = parseline(s);

        if (argv == nullptr || argv[0] == nullptr) { operator delete(argv); return 0; }

        if (exe_bultin_command(argv)) { operator delete(argv); return 0; }

        unified_cmd = find_cmd(argv[0]);
        unified_cmd_c = unified_cmd.c_str();
    }

    // block_io signals can stop shell
    // when child is at foreground, parent shell is at background
    sigprocmask(SIG_BLOCK, &block_io, nullptr);    

    sigset_t prev;
    block_all(&prev);

    shell_pgid = getpgid(0);
    pid_t pid = fork();
    
    if (pid == 0) // child
    {
        setpgid(0, 0);
        if (!is_async) // foreground
            tcsetpgrp(tty_fd, getpgrp());

        sigprocmask(SIG_SETMASK, &prev, nullptr);

        // program inside execve may use SIGTTIN/SIGTTOU
        // so just restore in child
        sigprocmask(SIG_UNBLOCK, &block_io, nullptr);

        // subshell
        if (task_type_is_subshell)
        {
            eval_tree_cd(i, *lexer_instance, true);
            exit(0);
        }

        // normal command
        execve(unified_cmd_c, argv, environ);

        // execve: if success, never returns

        cout << raw_input << flush;
        switch (errno)
        {
            case ENOTDIR:
                println(": A component of the path prefix is not a directory.");
                _exit(errno);
                break;
            
            case ENAMETOOLONG:
                println(": A component of a pathname exceeded 255 characters, or an entire path name exceeded 1023 characters.");
                _exit(errno);
                break;
            
            case ENOENT:
                println(": The new process file does not exist.");
                _exit(errno);
                break;

            case EACCES:
                println(": The new process file mode denies execute permission.");
                _exit(errno);
                break;

            default:
                println(": Error, code: {}", errno);
                _exit(errno);
        }
    }

    // parent

    operator delete(argv);
    argv = nullptr;

    exit_required_pid = pid;

    if (!is_async)  // foreground
    {   
        addjob(pid, FG, raw_input); 
        sigset_t prev_with_sigchld_blocked = prev;
        sigaddset(&prev_with_sigchld_blocked, SIGCHLD);
        sigprocmask(SIG_SETMASK, &prev_with_sigchld_blocked, NULL);
        waitfg(pid);
        sigprocmask(SIG_SETMASK, &prev, NULL);

        return exit_code;
    }
    else // background
    {
        addjob(pid, BG, raw_input);
        sigprocmask(SIG_SETMASK, &prev, NULL);
        cout << "[" << pid2jid(pid) << "] (" << pid << ") " << raw_input;

        // POSIX:
        // async command returns 0 immediately
        // TODO: recurring async 
        return 0;
    }



    return 0;
}

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
