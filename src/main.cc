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
#include "line/line_editor.h"
#include "signal_handler.h"

using namespace std;
using namespace ov4;

/* Global variables */
char sbuf[MAXLINE];         /* for composing sprintf messages */

/*
 * main - The shell's main routine 
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) 
{
    init(); 
    //test(argv);

    char cmdline[MAXLINE];

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */

    exe_option(argv);

    

    /* Initialize the job list */
    initjobs();

    /* Execute the shell's read/eval loop */
    while (1) {

        /* Read command line */
        cout << endl;
        int len = print_prompt();

        T_line_editor line_editor_instance;

        enable_raw();
        string s = line_editor_instance.readline(len);
        disable_raw();
        process_signal();
        cout << "\n\n\n\n" << endl;
        cout << s << endl;
        if (s == "quit")
            break;
        eval(s.c_str());

        fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
