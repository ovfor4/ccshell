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

#include "all.h"
#include "test.h"

using namespace std;
using namespace ov4;

/* Global variables */
char sbuf[MAXLINE];         /* for composing sprintf messages */

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    init(); 
    //test(argv);

    char c;
    char cmdline[MAXLINE];

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */

    exe_option(argv);

    // while ((c = getopt(argc, argv, "hvp")) != EOF) {
    //     switch (c) {
    //     case 'h':             /* print help message */
    //         usage();
	//     break;
    //     case 'v':             /* emit additional diagnostic info */
    //         verbose = true;
	//     break;
    //     case 'p':             /* don't print a prompt */
    //         emit_prompt = 0;  /* handy for automatic testing */
	//     break;
	// default:
    //         usage();
	// }
    // }

    

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
    print_prompt();

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
  
