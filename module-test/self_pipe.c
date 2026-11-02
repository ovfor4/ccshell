/*
 *
 * test self pipe
   $ gcc \
   module-test/self_pipe.c \
   -std=gnu23 \
   -O0 -ggdb3 -fno-omit-frame-pointer -fno-inline \
   -o module-test/self_pipe.out
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

#define TMP_SIZE 1024


typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void sigusr1_handler(int signum);
void sigusr2_handler(int signum);
int my_read(char *s, int len);
int my_poll(struct pollfd fds[], nfds_t nfds);


int self_pipe_fd[2] = {-1, -1};



void sigusr1_handler(int signum)
{
    char tmp = 'A';
    write(self_pipe_fd[1], &tmp, 1);
}

void sigusr2_handler(int signum) { /* empty */ }

int my_poll(struct pollfd fds[], nfds_t nfds)
{
    while (true)
    {
        int ret = poll(fds, nfds, -1);
        // if not signal'd
        if (!(ret == -1 && errno == EINTR))
            return ret;
    }
}

int my_read(char *s, int len)
{
    int max_fd;
    if (STDIN_FILENO > self_pipe_fd[0]) max_fd = STDIN_FILENO;
    else max_fd = self_pipe_fd[0];

    struct pollfd fds[2];

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    fds[1].fd = self_pipe_fd[0];
    fds[1].events = POLLIN;

    my_poll(fds, 2);

    if ((fds[0].revents) & POLLIN) // stdin
    {
        return read(STDIN_FILENO, s, len);
    }
    else if ((fds[1].revents) & POLLIN) // pipe
    {
        return read(self_pipe_fd[0], s, len);
    }
    
    return -2;
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

int main(int argc, char **argv)
{
    Signal(SIGUSR1, sigusr1_handler);
    Signal(SIGUSR2, sigusr2_handler);
    pipe(self_pipe_fd);
    int pid = getpid();
    printf("Running PID: %d\n", pid);
    printf("Do not enter anything\n");

    char tmp[TMP_SIZE];
    while (my_read(tmp, TMP_SIZE))
        printf("Hi\n");

    printf("Finished\n");
    return 0;
}