#pragma once

#include <string>
#include <vector>
#include <array>
#include <cstring>
#include <iostream>

namespace ov4
{

extern std::vector<std::string> builtin_cmd_list;

bool exe_bultin_command(char **argv);

}

namespace ov4::command
{

void quit();
void jobs();
void bg(char **argv);
void fg(char **argv);
void exit();

}
