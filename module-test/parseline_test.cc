/* 
 * test lexer/parseline
$ 
g++ \
    module-test/parseline_test.cc \
    src/lexer/parseline.cc \
    src/util/string.cc \
    -std=c++23 -Iinclude \
    -O0 -ggdb3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_ASSERTIONS \
    -o module-test/parseline_test.out
*/

#include <iostream>
#include <print>

#include "lexer/parseline.h"

using namespace std;

int main()
{
    int test_case = 0;
    println("start test");
    cin >> test_case;
    string s;

    switch (test_case)
    {
        case 0:
            s = "alpha beta";
            break;

        case 1:
            s = "alpha \'arg1 and arg2\'";
            break;

        case 2:
            s = "alpha \'arg1 \" arg2 arg3 \'";
            break;

        case 3:
            s = "alpha \'arg1 \"arg2 arg3\" arg4 \'";
            break;

        default:
            return 0;
    }
    char **p = ov4::parseline(s);
    if (p == nullptr)
    {
        println("Ooooops nullptr");
        return 0;
    }
    for (size_t i = 0; ; i++)
    {
        if (p[i] == nullptr)
            break;
        println("{}", p[i]);
    }
    return 0;
}
