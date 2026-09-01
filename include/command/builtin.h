#pragma once

#include <string>
#include <vector>
#include <array>
#include <cstring>
#include <iostream>
#include <map>
#include <unordered_map>
#include <functional>

#include "all.h"

namespace ov4
{

extern std::vector<std::string> builtin_cmd_list;

bool exe_bultin_command(char **argv);

}

namespace ov4::command
{

void quit(char **argv);
void jobs(char **argv);
void bg(char **argv);
void fg(char **argv);
void exit(char **argv);
void cd(char **argv);

}
