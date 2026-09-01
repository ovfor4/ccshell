#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <iostream>

#include "global.h"

namespace ov4
{

void string2argv(const std::vector<std::string> * const s, char **argv);
void argv2string(std::vector<std::string> *s, const char * const * argv);
    
}
