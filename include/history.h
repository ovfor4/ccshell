#pragma once

#include <string>
#include <vector>

namespace ov4
{

inline std::vector<std::string> history_vec;

void add_history(const std::string &s);
void clear_history();

}
