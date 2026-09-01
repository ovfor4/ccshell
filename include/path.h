#pragma once

#include <unistd.h>
#include <optional>
#include <string>
#include <unistd.h>
#include <iterator>
#include <iostream>


namespace ov4
{

extern std::string current_dir;

std::string find_cmd(char* s);
std::string find_cmd(std::string s);
std::string set_current_dir(std::string s);
std::string path_unifier(const std::string &s);

}
