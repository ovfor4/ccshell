#include "eval.h"

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
    

    sigset_t prev;
    block_all(&prev);

    pid_t pid = fork();
    string cmd = find_cmd(argv[0]);
    const char* cmd_c = cmd.c_str();
    if (pid == 0) // child
    {
        sigprocmask(SIG_SETMASK, &prev, nullptr);
        setpgid(0, 0);
        
        execve(cmd_c, argv, environ);

        atomic_print(cmdline, true);
        atomic_print(": Command not found\n");
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
