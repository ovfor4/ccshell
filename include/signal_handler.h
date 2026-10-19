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

inline constexpr int SIGNAL_MAX_NUM_P1 = 128;
inline std::atomic_char signal_mask[SIGNAL_MAX_NUM_P1];
bool check_signal_delivered(int sig);
void set_signal_delivered(int sig);
void unset_signal_delivered(int sig);

void signal_init();
handler_t *Signal(int signum, handler_t *handler);
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigquit_handler(int sig);
void sigwinch_handler(int sig);

void process_signal();
void process_sigchld();

int block_all(sigset_t *prev);
int block_handler(sigset_t *prev);

}
