#pragma once

#include <string>

namespace ov4
{
    
std::string make_color(const std::string s, const std::string color);

}

namespace ov4::color
{

const std::string
    black   = "30",
    red     = "31",
    green   = "32",
    yellow  = "33",
    blue    = "34",
    purple  = "35",
    cyan    = "36",
    white   = "37";

const std::string
    bright_black    = "90",
    bright_red      = "91",
    bright_green    = "92",
    bright_yellow   = "93",
    bright_blue     = "94",
    bright_purple   = "95",
    bright_cyan     = "96",
    bright_white    = "97";

}
