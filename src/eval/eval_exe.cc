#include "eval.h"

#include <string>

#include "error.h"
#include "lang_analysis/lexer_class.h"
#include "lang_analysis/ast.h"
#include "lang_analysis/token.h"
#include "lang_analysis/enum_type.h"
#include "lang_analysis/symbol_property.h"
#include "lang_analysis/parseline.h"
#include "util/io.h"

using namespace std;

extern char **environ;

namespace ov4
{

int eval_exe(const string &s, bool is_async)
{
    return eval_exe(s, is_async, nullptr, string::npos);
}

/*
 * receive a single command, and execute
 */
int eval_exe(const string &s, bool is_async, const T_lexer *lexer_instance, size_t i, const T_pipe &pi)
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
        eval_err(errno);
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

}