#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <iostream>

#include "global.h"

namespace ov4
{

void string2argv(const std::vector<std::string> &v, char **argv);
char **string2argv_alloc(const std::vector<std::string> &v);
void argv2string(std::vector<std::string> &v, const char * const * argv);
size_t argv_get_required_size(const std::vector<std::string> &v);
std::string safe_str(const char *s, const char *fallback = "");
void print_str_hex(const std::string &s);
    
}
