#pragma once

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

#include "global.h"

#define LOG !(ov4::GLOBAL_DEBUG && verbose) ? (void)0 : ov4::LogVoidify() & std::clog

namespace ov4 {

void atomic_print(char *s, bool ignore_wrap = false);
void atomic_print(int x);
void atomic_debug(char *s, bool ignore_wrap = false);
void atomic_debug(int x);
bool isnum(char *s);
void reverse(char str[], int length);
char *itoa(int num, char* str, int base);
int builtin_cmd(char *c_str);
void builtin_command_quit();

constexpr bool GLOBAL_DEBUG = true;
struct LogVoidify { void operator&(std::ostream&) const {} };

constexpr int DECIMAL = 10;
constexpr int LENGTH = 32;

}
