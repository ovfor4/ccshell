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
#include <type_traits>

#include "global.h"

#define LOG !(ov4::GLOBAL_DEBUG && verbose) ? (void)0 : ov4::LogVoidify() & std::clog

namespace ov4
{

struct LogVoidify { void operator&(std::ostream&) const {} };

inline constexpr int DECIMAL = 10;
inline constexpr int NUMBER_LENGTH = 70; // even enough for binary

// async-signal-safe
void safe_print(const char *s, bool ignore_wrap = false);

// async-signal-safe
void safe_print(int x);

// async-signal-safe
void safe_debug(const char *s, bool ignore_wrap = false);

// async-signal-safe
void safe_debug(int x);

// async-signal-safe
bool isnum(const char *s);

// async-signal-safe
void reverse(char *str, int length);

// async-signal-safe
char *itoa(int num, char* str, int base);

// async-signal-safe
// remove '\n' from the string
// when replacemen = '\0', '\n' is deleted (instead of really replacing by '\0' )
void wrap_eliminator(char *s, char replacement = '\0');

// async-signal-safe
void _write_helper(const char *s);

template <typename T>
void _safe_output_handler(T x)
{
    int errno_old = errno;

    if constexpr (std::is_same_v<T, int>) // int
    {
        char buffer[MAXLINE];
        itoa(x, buffer, DECIMAL);
        _write_helper(buffer);
    } 
    else if constexpr (std::is_same_v<T, char*> || std::is_same_v<T, const char*>) // C-style string
    {
        _write_helper(x);
    }
    else // ???
    {
        static_assert(false, "_safe_output_handler: unsopported type");
    }

    errno = errno_old;
}

inline void safe_output()
{
    ; // empty
}


// async-signal-safe output function
// example:
// safe_output("string", 123, variable);
template <typename T, typename ...T_arg>
void safe_output(T x, T_arg ...arg)
{
    int errno_old = errno;

    _safe_output_handler(x);
    safe_output(arg...);

    errno = errno_old;
}

void usage(void);
void unix_error(const char *msg);
void app_error(const char *msg);

}
