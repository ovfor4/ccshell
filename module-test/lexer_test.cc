// test lexer
/* 
$ 
g++ \
    module-test/lexer_test.cc \
    src/lexer/token.cc \
    src/lexer/ast.cc \
    src/lexer/symbol_property.cc \
    -std=c++23 -Iinclude -Ithird_party/magic_enum/include 
*/

#include <iostream>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <print>
#include <stdexcept>

#include "magic_enum/magic_enum.hpp"

#include "lexer/ast.h"
#include "lexer/enum_type.h"
#include "lexer/token.h"
#include "global.h"
#include "error.h"

using namespace std;
using namespace ov4;

constexpr int MAXLINE = 1024;


int main()
{
    verbose = true;
    int test_case = 13;
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
            s = "alpha || beta (gamma argument && (delta || epsilon second_argument & ) )";
            break;
        case 5:
            s = "alpha || beta && (gamma || epsilon)";
            break;
            
        // bracket matching test
        case 6:
            s = ")alpha && beta(";
            break;

        // basic scriptio continua
        // syntax is wrong but just to test tokenizer
        case 7:
            s = "((alpha&|&&beta)||gamma&&&delta)";
            break;

        // simple guard test
        case 8:
            s = "\'text\' &&";
            break;

        // guard test
        case 9:
            s = "\'realOperator\' && \"fake&&\" real ||";
            break;

        // guard test
        // with space
        case 10:
            s = "\'This should be 1 token\' symbol && text \'&&\'";
            break;

        // missing test
        // missing right
        case 11:
            s = "alpha&&";
            break;
        
        // missing test
        // missing left
        case 12:
            s = "&&left";
            break;

        // existance test
        // exist right
        case 13:
            s = "alpha&beta";
            break;

        default:
            return 0;
    }

    try
    {
        T_lexer lexer_instance;
        lexer_instance.tokenizer(s);
        cout << "---------- tokenizer ----------" << endl;
        for (auto c : lexer_instance.token)
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
                println("{} TEXT .{}.", c.bracket_depth, c.text);
        }

        cout << "---------- parse ----------" << endl;

        lexer_instance.parse(0, lexer_instance.token.size(), lexer_instance.alloc_ast());

        cout << "---------- AST ----------" << endl;
        for (size_t i = 0; i < lexer_instance.ast.size(); i++)
        {
            print("{}: type: {} left: {} right: {}", i, magic_enum::enum_name(lexer_instance.ast[i].token_type), lexer_instance.ast[i].left, lexer_instance.ast[i].right);
            if (lexer_instance.ast[i].token_type == TEXT)
                print(" command: {}", lexer_instance.ast[i].command_text);
            println();
        }

    }
    catch(const T_error &e)
    {
        cerr << "Error: " << magic_enum::enum_name(e.code) << endl;
        cerr << "what: " << e.what() << endl;
    }
    

    

    
    return 0;
}
