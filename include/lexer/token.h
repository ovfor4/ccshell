#pragma once

#include <string>

#include "parser/enum_type.h"
#include "parser/shared.h"

using namespace std; // remove this

namespace ov4
{

class T_token
{
public:
    T_token() = default;

    enum_token_type token_type;
    string text;
    guard_type guard = GUARD_OFF;
    int bracket_depth = 0;
};

size_t find_non_space_previous(string s, size_t i)
{
    size_t len = s.size(), backup = i;
    i--; // skip current char
    for ( ; i < len; i--)
        if (s[i] != ' ')
            return i+1; // because region is [x, y) 

    return backup;
}

string trim_space(const string &s)
{
    size_t b = s.find_first_not_of(" \t");
    if (b == string::npos)
        return "";
    size_t e = s.find_last_not_of(" \t");
    return s.substr(b, e - b + 1);
}


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

    string trimed = trim_space(sub);
    if (trimed == "")   return;

    tmp.token_type = TEXT;
    tmp.text = trimed;
    token.push_back(tmp);
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

}
