#pragma once

#include <vector>
#include <string>

namespace ov4
{

extern int verbose;

constexpr int MAXLINE = 1024;
constexpr int MAXARGS = 128;

extern std::vector<std::string> builtin_cmd_list;

}
