#include <iostream>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <print>

#include "parser/shared.h"
#include "parser/ast.h"
#include "parser/enum_type.h"
#include "parser/token.h"

using namespace std;
using namespace ov4;

constexpr int MAXLINE = 1024;




int main()
{
    int test_case = 4;
    string s;

    switch (test_case)
    {
        case 0:
            s = "hello | hi || foo && bar || baz &";
            break;
        case 1:
            s = "hi";
            break;
        case 2:
            s = "&&";
            break;
        case 3:
            s = "hi (hello || foo)";
            break;
        case 4:
            s = "alpha || beta (gamma && (delta || epsilon & ) )";
            break;
        default:
            return 0;
    }



    tokenizer(s);
    cout << "---------- tokenizer ----------" << endl;
    for (auto c : token)
    {
        if (c.token_type == LOGIC_AND)
            println("{} LOGIC_AND", c.bracket_depth);
        else if (c.token_type == LOGIC_OR)
            println("{} LOGIC_OR", c.bracket_depth);
        else if (c.token_type == ASYNC)
            println("{} ASYNC", c.bracket_depth);
        else if (c.token_type == PIPE)
            println("{} PIPE", c.bracket_depth);
        else if (c.token_type == LEFT_BRACKET)
            println("{} LEFT_BRACKET", c.bracket_depth);
        else if (c.token_type == RIGHT_BRACKET)
            println("{} RIGHT_BRACKET", c.bracket_depth);
        else if (c.token_type == TEXT)
            println("{} TEXT {}", c.bracket_depth, c.text);
    }

    cout << "---------- parse ----------" << endl;

    parse(0, token.size(), alloc_ast());
    return 0;
}
