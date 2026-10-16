/*
 *
 * test self pipe
   $ g++ \
   module-test/self_pipe.cc \
   -std=c++26 \
   -O0 -ggdb3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_ASSERTIONS \
   -o module-test/self_pipe.out
 */

#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void sigusr1_handler(int signum);
int my_read(char *s, int len);

int self_pipe_fd[2] = {-1, -1};



void sigusr1_handler(int signum)
{
    char tmp = 'A';
    write(self_pipe_fd[1], &tmp, 1);
}

int my_read(char *s, int len)
{
    int max_fd;
    if (STDIN_FILENO > self_pipe_fd[0]) max_fd = STDIN_FILENO;
    else max_fd = self_pipe_fd[0];
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(STDIN_FILENO, &rset);
    FD_SET(self_pipe_fd[0], &rset);
    select(max_fd+1, &rset, nullptr, nullptr, nullptr);
    if (FD_ISSET(self_pipe_fd[0], &rset))
    {
        return read(self_pipe_fd[0], s, len);
    }
    else if (FD_ISSET(STDIN_FILENO, &rset))
    {
        return read(STDIN_FILENO, s, len);
    }
    return -1;
}


handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    sigaction(signum, &action, &old_action);
    return (old_action.sa_handler);
}

int main()
{
    Signal(SIGUSR1, sigusr1_handler);
    pipe(self_pipe_fd);
    cout << "Running PID: " << getpid() << endl;
    cout << "Do not enter anything" << endl;

    constexpr int tmp_size = 1024;
    char tmp[tmp_size];
    while (my_read(tmp, tmp_size))
        cout << "Hi" << endl;

    cout << "Finished" << endl;
    return 0;
}