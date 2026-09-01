#include "util/color.h"

#include <string>

#include "all.h"

using namespace std;

namespace ov4
{

string make_color(const string s, const string_view color)
{
    return "\e[0;" + (string)color + "m" + s + "\e[0m";
}
    
}
