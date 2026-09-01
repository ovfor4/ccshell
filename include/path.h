#pragma once

#include <unistd.h>
#include <optional>
#include <string>
#include <unistd.h>
#include <iterator>
#include <iostream>

#include "all.h"


namespace ov4
{

std::string find_cmd(const char* s);
std::string find_cmd(const std::string &s);
std::string path_unifier(const std::string &s, bool is_cmd = false);
std::string get_current_dir();
int set_current_dir(const std::string &s);

}
