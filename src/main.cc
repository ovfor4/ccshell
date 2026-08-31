/* 
 * tsh - A tiny shell program with job control
 * 
 * <Put your name and login ID here>
 */
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

#include "util.h"
#include "job.h"

using namespace std;
using namespace ov4;

/* Misc manifest constants */


/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
char sbuf[MAXLINE];         /* for composing sprintf messages */




vector<string> builtin_cmd_list = {"quit", "fg", "bg", "jobs"};
sigset_t BLOCK_HANDLER;
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char *c_str);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);


void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

// built-in commands
void builtin_command_quit();

inline int block_all(sigset_t*);
inline int block_handler(sigset_t*);

void init();

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    init();
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = true;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
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
    char arg[MAXARGS][MAXLINE];
    char *argv[MAXARGS];
    for (int i = 0 ; i < MAXARGS; i++)
    {
        argv[i] = &arg[i][0];
    }
    int ground = parseline(cmdline, argv);

    if (argv[0] == nullptr) return;
    
    if (builtin_cmd(argv[0])) 
    {   
        LOG << "built-in" << endl;
        if (strcmp(argv[0], "quit") == 0) builtin_command_quit();
        else if (strcmp(argv[0], "jobs") == 0) listjobs(jobs);
        else if (strcmp(argv[0], "fg") == 0) do_bgfg(argv);
        else if (strcmp(argv[0], "bg") == 0) do_bgfg(argv);
        else cerr << "built-in but not supported" << endl;
        return;
    }

    sigset_t prev;
    block_all(&prev);

    pid_t pid = fork();
    if (pid == 0) // child
    {
        sigprocmask(SIG_SETMASK, &prev, nullptr);
        setpgid(0, 0);
        
        execve(argv[0], argv, environ);

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

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');
    }
    else {
	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char *c_str) 
{
    string s(c_str);
    for (auto c : builtin_cmd_list)
        if (c == s)
            return 1;
    return 0;     /* not a builtin command */
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
            if (kill(-(j->pid), SIGCONT) < 0) 
            {
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
    LOG << "finished fg " << endl;
    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    int errno_backup = errno;

    sigset_t prev;
    block_handler(&prev);

    int status, pid;
    atomic_debug("SIGCHLD\n");

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        if (WIFSIGNALED(status) || WIFSTOPPED(status))
        {
            int jid = pid2jid(pid);
            atomic_print("Job [");
            atomic_print(jid);
            atomic_print("] (");
            atomic_print(pid);
            atomic_print(") ");
            if (!WIFSTOPPED(status) && WTERMSIG(status) == SIGINT)
                atomic_print("terminated");
            else if (WIFSTOPPED(status))
                atomic_print("stopped");
            else 
                atomic_print("idk");
            atomic_print(" by signal ");
            if (WIFSTOPPED(status))
                atomic_print(WSTOPSIG(status));
            else
                atomic_print(WTERMSIG(status));
            atomic_print("\n");
        } 

        if (WIFSTOPPED(status))
        {
            atomic_debug("suspended pid: ");
            atomic_debug(pid);
            atomic_debug("\n");
            sigset_t prev_inner;
            block_all(&prev_inner);
            job_suspend(jobs, pid);
            sigprocmask(SIG_SETMASK, &prev_inner, nullptr);
        }
        else {
            atomic_debug("terminated pid: ");
            atomic_debug(pid);
            sigset_t prev_inner;
            block_all(&prev_inner);
            deletejob(jobs, pid);
            sigprocmask(SIG_SETMASK, &prev_inner, nullptr);
        }
    }

    sigprocmask(SIG_SETMASK, &prev, nullptr);

    errno = errno_backup;
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    int errno_backup = errno;

    sigset_t prev;
    block_handler(&prev);

    int pid = fgpid(jobs);
    if (pid != 0)
    {
        kill(-pid, SIGINT);
    }

    sigprocmask(SIG_SETMASK, &prev, nullptr);

    errno = errno_backup;
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    int errno_backup = errno;

    sigset_t prev;
    block_handler(&prev);

    int pid = fgpid(jobs);
    if (pid != 0)
    {
        kill(-pid, SIGTSTP);
    }

    sigprocmask(SIG_SETMASK, &prev, nullptr);

    errno = errno_backup;
    return;
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */

/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}


void builtin_command_quit()
{
    exit(0);
}

// return true if suspended, otherwise false


inline int block_all(sigset_t *prev)
{
    sigset_t set;
    sigfillset(&set);
    return sigprocmask(SIG_SETMASK, &set, prev);
}

inline int block_handler(sigset_t *prev)
{
    return sigprocmask(SIG_BLOCK, &BLOCK_HANDLER, prev);
}

void init()
{
    sigemptyset(&BLOCK_HANDLER);
    sigaddset(&BLOCK_HANDLER, SIGINT);
    sigaddset(&BLOCK_HANDLER, SIGTSTP);
    sigaddset(&BLOCK_HANDLER, SIGCHLD);
}
