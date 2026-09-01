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

namespace ov4
{

struct LogVoidify { void operator&(std::ostream&) const {} };

inline constexpr int DECIMAL = 10;
inline constexpr int LENGTH = 32;

void safe_print(char *s, bool ignore_wrap = false);
void safe_print(int x);
void safe_debug(char *s, bool ignore_wrap = false);
void safe_debug(int x);
bool isnum(char *s);
void reverse(char str[], int length);
char *itoa(int num, char* str, int base);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);

}
