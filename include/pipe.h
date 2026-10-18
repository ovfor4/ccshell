#pragma once


namespace ov4
{

inline int self_pipe_fd[2] = {-1, -1};

constexpr unsigned char SELF_PIPE_BYTE = 1;

void self_pipe_init();
int self_pipe_write();

}