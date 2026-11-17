/*
 * test path finder
 $ g++ \
   module-test/ls_test.cc \
   src/util/dir.cc \
   -DUTIL_DIR_VERBOSE \
   -std=c++26 -Iinclude -Ithird_party/magic_enum/include \
   -O0 -ggdb3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_ASSERTIONS \
   -o module-test/ls_test.out && module-test/ls_test.out
*/

#include <iostream>
#include <print>
#include <sys/stat.h>

#include "util/dir.h"

using namespace std;
using namespace ov4;
using namespace ov4::dir;

int main(int argc, char **argv)
{

    auto v = lsv("/", false, false);
    for (auto &c : v)
    {
        string type;
        if (S_ISREG(c.mode))
            type = "Regular";
        else if (S_ISDIR(c.mode))
            type = "Directory";
        else
            type = "Other";
        println("{} {}", c.path, type);
    }
}
