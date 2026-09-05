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
    TEXT,
    SINGLE_QUOTATION,
    DOUBLE_QUOTATION,
    LOGIC_AND,
    LOGIC_OR,
    ASYNC,
    PIPE,
    EXPAND,
};

class T_token
{
public:
    T_token() = default;

    enum_token_type token_type;
    string text;
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
    if (c == '&' || c == '|')
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
void token_push(string s, size_t current, size_t prev, bool is_symbol_prev)
{
    T_token tmp;
    string sub = s.substr(prev, current - prev);
        cout << "substr " << sub << endl;

    if (is_symbol_prev)
    {
        if (is_symbol_same(sub, "&&"))
        {
            cout << "&& symbol" << endl;
            tmp.token_type = LOGIC_AND;
            token.push_back(tmp);
        }
        else if (is_symbol_same(sub, "||"))
        {
            cout << "|| symbol" << endl;
            tmp.token_type = LOGIC_OR;
            token.push_back(tmp);
        }
        else if (is_symbol_same(sub, "&"))
        {
            cout << "& symbol" << endl;
            tmp.token_type = ASYNC;
            token.push_back(tmp);
        }
        else if (is_symbol_same(sub, "|"))
        {
            cout << "| symbol" << endl;
            tmp.token_type = PIPE;
            token.push_back(tmp);
        }
        else
        {
            cerr << "ERROR" << endl;
        }
        return;
    }

    tmp.token_type = TEXT;
    tmp.text = sub;
    token.push_back(tmp);
}

void tokenizer(string s)
{
    if (s.size() == 0) return;

    size_t len = s.size();
    size_t prev = 0;
    size_t token_vector_index = 0;
    bool is_in_symbol = false;

    if (is_symbol(s[0]))
    {
        is_in_symbol = true;
    }
    for (size_t i = 1; i < len; i++)
    {
        println("prev {} current char {}", prev, s[i]);
        // turn from symbol to text
        if (is_in_symbol && !is_symbol(s[i]))
        {
            cout << s[i] <<" symbol -> text" << endl;
            is_in_symbol = false;
            token_push(s, i, prev, true);
            prev = i;
            continue;
        }

        // turn from text to symbol
        if (!is_in_symbol && is_symbol(s[i]))
        {
            cout << s[i] << " text -> symbol" << endl;
            is_in_symbol = true;
            token_push(s, i, prev, false);
            prev = i;
            continue;
        }

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

    // final round text
    if (!is_symbol(s[len - 1]))
    {
        cout << s[len - 1] << " final text" << endl;
        token_push(s, len, prev, false);
    }

    // final round symbol
    if (is_symbol(s[len - 1]))
    {
        cout << s[len - 1] << " final symbol" << endl;
        token_push(s, len, prev, true);
    }
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

int get_priority(enum_token_type x)
{
    if (x == LOGIC_AND || x == LOGIC_OR)
        return 1000;
    else if (x == PIPE || x == ASYNC)
        return 500;
    return 0;
}


int main()
{
    int test_case = 0;
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
        default:
            return 0;
    }



    tokenizer(s);
    cout << "----------" << endl;
    for (auto c : token)
    {
        if (c.token_type == LOGIC_AND)
            cout << "LOGIC_AND" << endl;
        else if (c.token_type == LOGIC_OR)
            cout << "LOGIC_OR" << endl;
        else if (c.token_type == ASYNC)
            cout << "ASYNC" << endl;
        else if (c.token_type == PIPE)
            cout << "PIPE" << endl;
        else if (c.token_type == TEXT)
            cout << "TEXT " << c.text << endl;
    }
    return 0;
}
