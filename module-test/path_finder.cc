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

int main(int argc, char **argv)
{
    file_property_t file_property;
    
    switch (argc)
    {
        case 1:
        file_property = {
            .dir_type = dir_type_t::CUSTOM,
            .pattern = "/bin/ba*",
        };
        break;


        default:
        file_property = {
            .dir_type = dir_type_t::PATH,
            .pattern = "ba*",
        };

    }

    auto v = fcv(file_property);
    for (auto &c : v)
    {
        println("{}", c.ret);
    }
}
