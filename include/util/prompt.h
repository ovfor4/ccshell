#pragma once

#include <iostream>

#include "all.h"

namespace ov4
{

inline constexpr std::string_view prompt_normal = "$";
inline constexpr std::string_view prompt_root = "#";
extern bool emit_prompt;

void print_prompt();
std::string get_prompt();

}
