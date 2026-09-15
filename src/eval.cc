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
#include "lexer/lexer_class.h"
#include "lexer/ast.h"
#include "lexer/token.h"
#include "lexer/enum_type.h"
#include "lexer/symbol_property.h"
#include "handler.h"

using namespace std;

extern char **environ;

namespace ov4
{

int eval_exe(string s, bool is_aync)
{
    loggerln("eval_exe: receive {}", s);

    char cmdline[MAXLINE];
    strcpy(cmdline, s.c_str());
    char arg[MAXARGS][MAXLINE];
    char *argv[MAXARGS];
    for (int i = 0 ; i < MAXARGS; i++)
    {
        argv[i] = &arg[i][0];
    }
    parseline(cmdline, argv);

    if (exe_bultin_command(argv)) return 0;

    // block_io signals can stop shell
    // when child is at foreground, parent shell is at background
    sigprocmask(SIG_BLOCK, &block_io, nullptr);

    sigset_t prev;
    block_all(&prev);

    string cmd = find_cmd(argv[0]);
    const char* cmd_c = cmd.c_str();

    shell_pgid = getpgid(0);
    pid_t pid = fork();
    
    if (pid == 0) // child
    {
        setpgid(0, 0);
        if (!is_aync) // foreground
            tcsetpgrp(tty_fd, getpgrp());

        sigprocmask(SIG_SETMASK, &prev, nullptr);

        // program inside execve may use SIGTTIN/SIGTTOU so just restore in child
        sigprocmask(SIG_UNBLOCK, &block_io, nullptr);
        execve(cmd_c, argv, environ);

        wrap_eliminator(cmdline);
        cout << cmdline << flush;
        switch (errno)
        {
            case ENOTDIR:
                println(": A component of the path prefix is not a directory.");
                exit(errno);
                break;
            
            case ENAMETOOLONG:
                println(": A component of a pathname exceeded 255 characters, or an entire path name exceeded 1023 characters.");
                exit(errno);
                break;
            
            case ENOENT:
                println(": The new process file does not exist.");
                exit(errno);
                break;

            case EACCES:
                println(": The new process file mode denies execute permission.");
                exit(errno);
                break;

            default:
                println(": Error, code: {}", errno);
                exit(errno);
        }
    }

    // parent

    exit_required_pid = pid;

    if (!is_aync)  // foreground
    {   
        addjob(pid, FG, cmdline); 
        sigset_t prev_with_sigchld_blocked = prev;
        sigaddset(&prev_with_sigchld_blocked, SIGCHLD);
        sigprocmask(SIG_SETMASK, &prev_with_sigchld_blocked, NULL);
        waitfg(pid);
        sigprocmask(SIG_SETMASK, &prev, NULL);
    }
    else // background
    {
        addjob(pid, BG, cmdline);
        sigprocmask(SIG_SETMASK, &prev, NULL);
        cout << "[" << pid2jid(pid) << "] (" << pid << ") " << cmdline;
    }



    return 0;
}

void eval_tree_cd(size_t i, const T_lexer &lexer_instance)
{
    // TODO: async
    if (lexer_instance.ast[i].token_type == TEXT)
    {
        eval_exe(lexer_instance.ast[i].command_text, false);
        return;
    }
    if (lexer_instance.ast[i].left != string::npos)
    {
        eval_tree_cd(lexer_instance.ast[i].left, lexer_instance);
    }

    if (lexer_instance.ast[i].right != string::npos)
    {
        eval_tree_cd(lexer_instance.ast[i].right, lexer_instance);
    }
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
void eval(char *cmdline) 
{
    string s = cmdline;
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

    eval_tree_cd(0, lexer_instance);
    return;
}

}
