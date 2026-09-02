#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <utility>

#include "option.h"

namespace ov4
{

inline const std::unordered_map<std::string, std::function<void(std::string s)>> option_list = 
{
    {"-v", ov4::option::v},
};

void exe_option(char **c_argv);
std::pair<std::string, std::string> option_split(std::string s);
    
}
