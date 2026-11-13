#pragma once

#include <cstdio>

inline void cleanup_free_argv(char ***p)
{
    free(*p);
}
