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
void eval(char *cmdline) 
{
    char arg[MAXARGS][MAXLINE];
    char *argv[MAXARGS];
    for (int i = 0 ; i < MAXARGS; i++)
    {
        argv[i] = &arg[i][0];
    }
    int ground = parseline(cmdline, argv);

    if (argv[0] == nullptr) return;
    if (exe_bultin_command(argv)) return;
    
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
        if (!ground) // foreground
            tcsetpgrp(tty_fd, getpgrp());

        sigprocmask(SIG_SETMASK, &prev, nullptr);

        // program inside execve may use SIGTTIN/SIGTTOU so just restore in child
        sigprocmask(SIG_UNBLOCK, &block_io, nullptr);
        
        execve(cmd_c, argv, environ);

        safe_print(cmdline, true);
        safe_print(": Command not found\n");
        exit(-1); // if no command is found
    }

    // parent

    if (!ground)  // foreground
    {   
        addjob(jobs, pid, FG, cmdline); 
        sigset_t prev_with_sigchld_blocked = prev;
        sigaddset(&prev_with_sigchld_blocked, SIGCHLD);
        sigprocmask(SIG_SETMASK, &prev_with_sigchld_blocked, NULL);
        waitfg(pid);
        sigprocmask(SIG_SETMASK, &prev, NULL);
    }
    else // background
    {
        addjob(jobs, pid, BG, cmdline);
        sigprocmask(SIG_SETMASK, &prev, NULL);
        cout << "[" << pid2jid(pid) << "] (" << pid << ") " << cmdline;
    }



    return;
}

}
