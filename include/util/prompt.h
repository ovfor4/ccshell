#pragma once

#include <iostream>

#include "all.h"

namespace ov4
{
    constexpr std::string_view prompt_normal = "$";
    constexpr std::string_view prompt_root = "#";
    constexpr size_t hostname_size = 128;
    extern bool emit_prompt;

    void print_prompt();
    std::string get_prompt();
}
