#include "ground.h"

using namespace std;

namespace ov4
{

pid_t shell_pgid;
int tty_fd;

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    sigset_t prev_with_sigchld_unblocked;
    // get current blocked signals
    sigprocmask(0, nullptr, &prev_with_sigchld_unblocked);
    sigdelset(&prev_with_sigchld_unblocked, SIGCHLD);
    LOG << "waiting for pid "<< fgpid(jobs) << endl;
    while (fgpid(jobs) != 0)
        sigsuspend(&prev_with_sigchld_unblocked);

    // set shell to foreground again
    tcsetpgrp(tty_fd, shell_pgid);
    sigprocmask(SIG_BLOCK, &block_io, nullptr);

    LOG << "finished fg " << endl;
    return;
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    if ((strcmp(argv[0], "fg") == 0) || (strcmp(argv[0], "bg") == 0))
    {
        int x;
        job_t *j = nullptr;
        sigset_t prev;
        if (argv[1] == nullptr)
        {
            cerr << ((strcmp(argv[0], "fg") == 0) ? "fg" : "bg") << " command requires PID or %jobid argument" << endl;
            return;
        }
        if (argv[1][0] == '%') // jid provided
        {
            if (!isnum(&argv[1][1]))
            {
                cerr << ((strcmp(argv[0], "fg") == 0) ? "fg" : "bg") << ": argument must be a PID or %jobid" << endl;
                return;
            }
            x = atoi(&argv[1][1]); // eat %
            j = getjobjid(jobs, x);
            if (j == nullptr)
            {
                cerr << argv[1] << ": No such job" << endl;
                return;
            }
        } else { // pid
            if (!isnum(&argv[1][0]))
            {
                cerr << ((strcmp(argv[0], "fg") == 0) ? "fg" : "bg") << ": argument must be a PID or %jobid" << endl;
                return;
            }
            x = atoi(&argv[1][0]);
            j = getjobpid(jobs, x);
            if (j == nullptr)
            {
                cerr << "(" << argv[1] << "): No such process" << endl;
                return;
            }
        }


        block_all(&prev);
        if (strcmp(argv[0], "fg") == 0) // fg
        {
            LOG << "fg" << endl;
            j->state = FG;
            
            sigset_t prev_with_sigchld_blocked = prev;
            sigaddset(&prev_with_sigchld_blocked, SIGCHLD);
            sigprocmask(SIG_SETMASK, &prev_with_sigchld_blocked, NULL);

            sigprocmask(SIG_BLOCK, &block_io, nullptr);
            tcsetpgrp(tty_fd, j->pid);

            if (kill(-(j->pid), SIGCONT) < 0) 
            {
                tcsetpgrp(tty_fd, shell_pgid);
                cerr << "Cannot continue process " << j->pid;
                sigprocmask(SIG_SETMASK, &prev, NULL);
                return;
            }

            waitfg(j->pid);

            sigprocmask(SIG_SETMASK, &prev, NULL);
        } else // bg
        {
            LOG << "bg" << endl;
            j->state = BG;
            kill(-(j->pid), SIGCONT);
            sigprocmask(SIG_SETMASK, &prev, NULL);
            // cmdline has nerd '\n' at the end
            // so just flush
            cout << "[" << j->jid << "] (" << j->pid << ") " << j->cmdline << flush;
        }
    }
    else 
        cerr << "not fg/bg" << endl;
}

}
