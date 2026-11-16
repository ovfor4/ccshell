/*
 * test path finder
 $ g++ \
   module-test/path_finder.cc \
   src/util/path.cc \
   -DUTIL_PATH_VERBOSE \
   -std=c++26 -Iinclude -Ithird_party/magic_enum/include \
   -O0 -ggdb3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_ASSERTIONS \
   -o module-test/path_finder.out && module-test/path_finder.out
*/

#include <iostream>
#include <print>

#include "util/path.h"

using namespace std;
using namespace ov4;
using namespace ov4::path;

int main()
{
    file_property_t file_property;
    auto v = fcv(file_property);
    for (auto &c : v)
    {
        println("{}", c.ret);
    }
}
