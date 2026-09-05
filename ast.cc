#include <iostream>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <print>

using namespace std;

constexpr int MAXLINE = 1024;
size_t ast_index = 0;

enum enum_token_type
{
    TEXT,                   //    foo
    SINGLE_QUOTATION,       //    '
    DOUBLE_QUOTATION,       //    "
    LOGIC_AND,              //    &&
    LOGIC_OR,               //    ||
    ASYNC,                  //    &
    PIPE,                   //    |    
    SEMICOLON,              //    ;
    EXPAND,                 //    $
    LEFT_BRACKET,           //    (
    RIGHT_BRACKET,          //    )
};

enum guard_type
{
    GUARD_OFF,
    GUARD_WEAK,
    GUARD_STRONG,
};

class T_token
{
public:
    T_token() = default;

    enum_token_type token_type;
    string text;
    guard_type guard = GUARD_OFF;
    int bracket_depth = 0;
};

class T_ast
{
public:
    T_ast() = default;

    enum_token_type token_type;
    string command_text;
    size_t left;
    size_t right;
};



vector<T_ast> ast;
vector<T_token> token;

bool is_symbol(char c)
{
    if (c == '&' || c == '|' || c == '(' || c == ')')
        return true;
    return false;
}

bool is_symbol_same(string x, string y)
{
    string tmp = "";
    for (size_t i = 0; i < x.size(); i++)
    {
        if (x[i] != ' ')
            tmp += x[i];
    }
    return (tmp == y);
}

// handle [prev, current)
void token_push(string s, size_t current, size_t prev, int bracket_depth, bool is_symbol_prev)
{
    T_token tmp;
    string sub = s.substr(prev, current - prev);
        cout << "substr " << sub << endl;

    tmp.bracket_depth = bracket_depth;
    if (is_symbol_prev)
    {
        if (is_symbol_same(sub, "&&"))
        {
            cout << "&& symbol" << endl;
            tmp.token_type = LOGIC_AND;
        }
        else if (is_symbol_same(sub, "||"))
        {
            cout << "|| symbol" << endl;
            tmp.token_type = LOGIC_OR;
        }
        else if (is_symbol_same(sub, "&"))
        {
            cout << "& symbol" << endl;
            tmp.token_type = ASYNC;
        }
        else if (is_symbol_same(sub, "|"))
        {
            cout << "| symbol" << endl;
            tmp.token_type = PIPE;
        }
        else if (is_symbol_same(sub, "("))
        {
            cout << "( symbol" << endl;
            tmp.token_type = LEFT_BRACKET;
        }
        else if (is_symbol_same(sub, ")"))
        {
            cout << ") symbol" << endl;
            tmp.token_type = RIGHT_BRACKET;
        }
        else
        {
            cerr << "ERROR: is symbol but not symbol" << endl;
            return;
        }
        token.push_back(tmp);
        return;
    }

    tmp.token_type = TEXT;
    tmp.text = sub;
    token.push_back(tmp);
}

void bracket_depth_changer(char c, int &bracket_depth)
{
    if (c == '(')
    {
        bracket_depth++;
        println("changed depth: {}", bracket_depth);
        return;
    }
    if (c == ')')
    {
        bracket_depth--;
        println("changed depth: {}", bracket_depth);
        return;
    }
}

int tokenizer(string s)
{
    if (s.size() == 0) return -1;

    size_t len = s.size();
    size_t prev = 0;
    size_t token_vector_index = 0;
    bool is_in_symbol;
    int bracket_depth = 0;

    if (is_symbol(s[0]))
        is_in_symbol = true;
    else
        is_in_symbol = false;
    for (size_t i = 0; i < len; i++)
    {
        println("---");
        println("prev {} current char {}", prev, s[i]);


        // turn from symbol to text
        if (is_in_symbol && !is_symbol(s[i]))
        {
            cout << s[i] <<" symbol -> text" << endl;
            is_in_symbol = false;
            token_push(s, i, prev, bracket_depth, true);
            prev = i;
            bracket_depth_changer(s[i], bracket_depth);
            continue;
        }

        // turn from text to symbol
        if (!is_in_symbol && is_symbol(s[i]))
        {
            cout << s[i] << " text -> symbol" << endl;
            is_in_symbol = true;
            token_push(s, i, prev, bracket_depth, false);
            prev = i;
            bracket_depth_changer(s[i], bracket_depth);
            continue;
        }

        bracket_depth_changer(s[i], bracket_depth);

        // text continue
        if (!is_in_symbol && !is_symbol(s[i]))
        {
            cout << s[i] <<" still text" << endl;
            continue;
        }

        // symbol continue
        if (is_in_symbol && is_symbol(s[i]))
        {
            cout << s[i] <<" still symbol" << endl;
            continue;
        }
    }

    if (bracket_depth != 0)
    {
        cerr << "Brackets are not paired" << endl;
        return -1;
    }

    // final round text
    if (!is_symbol(s[len - 1]))
    {
        cout << s[len - 1] << " final text" << endl;
        token_push(s, len, prev, 0, false);
    }

    // final round symbol
    if (is_symbol(s[len - 1]))
    {
        cout << s[len - 1] << " final symbol" << endl;
        token_push(s, len, prev, 0, true);
    }
    return 0;
}

size_t alloc_ast()
{
    ast.emplace_back();
    return ast_index++;
}

void parse(size_t cmd_begin, size_t cmd_end, size_t ast_vec)
{
    ;
}

int get_pivot_order(enum_token_type x)
{
    switch(x)
    {
        case ASYNC:
        case SEMICOLON:
            return 1000;
        
        case LOGIC_AND:
        case LOGIC_OR:
            return 500;

        case PIPE:
            return 100;
            
        default:
            return 0;
    }
    return 0;
}


int main()
{
    int test_case = 3;
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
        default:
            return 0;
    }



    tokenizer(s);
    cout << "----------" << endl;
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
    return 0;
}
