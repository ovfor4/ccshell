#pragma once

#include <signal.h>
#include <atomic>

#include "all.h"

namespace ov4
{

typedef void handler_t(int);

inline sigset_t 
    block_sig_TTOU,
    block_job,
    block_io;

inline std::atomic_int exit_code, exit_required_pid;

inline constexpr int SIGNAL_EXIT_CODE_BASE = 128;

void signal_init();
handler_t *Signal(int signum, handler_t *handler);
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigquit_handler(int sig);

int block_all(sigset_t *prev);
int block_handler(sigset_t *prev);

}
