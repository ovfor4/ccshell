#pragma once

#include <string>
#include <vector>

namespace ov4
{

inline std::vector<std::string> history_vec;

inline size_t history_index;

void add_history(const std::string &s);
void clear_history();

std::size_t get_last_index();

}
