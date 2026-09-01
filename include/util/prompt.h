#pragma once

#include <iostream>

#include "all.h"

namespace ov4
{
    constexpr std::string_view prompt = "$";
    extern bool emit_prompt;

    void print_prompt();
}
