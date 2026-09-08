#pragma once

#include <string>

#include "lexer/enum_type.h"
#include "lexer/shared.h"
#include "lexer/symbol_property.h"

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

string trim_space(const string &s)
{
    size_t b = s.find_first_not_of(" \t");
    if (b == string::npos)
        return "";
    size_t e = s.find_last_not_of(" \t");
    return s.substr(b, e - b + 1);
}


bool is_single_symbol(char c)
{
    string s = " ";
    s[0] = c;
    if (symbol_property.contains(s))
        return true;
    return false;
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
        if (bracket_depth < 0)
        {
            cerr << "Ooooops bracket unmatched, quitting" << endl;
            exit(-1); // TODO: std::expected or what, rather than quitting
        }
        return;
    }
}


bool token_continue(string s, char next)
{
    println("token_continue: receiving {} {}", s, next);
    // s is empty, so next can be part of the token
    if (s.size() == 0) return true;

    // text -> text/symbol
    if (!is_single_symbol(s[s.size()-1]))
    {
        // text -> symbol
        if (is_single_symbol(next))
            return false;

        // text still
        return true;
    }

    // symbol -> symbol
    T_property tmp;
    if (symbol_property.contains(s))
    {
        tmp = symbol_property[s];
        if (tmp.continuable)
        {
            string prev_and_next = s + next;
            if (symbol_property.contains(prev_and_next))
                return true;
            return false;
        }
        return false;
    }
    else
    {
        println("symbol not found in map");
        return false;
    }
}

// handle [prev, current)
void token_push(string push_s, int bracket_depth)
{
    T_token tmp;
    tmp.bracket_depth = bracket_depth;
    string trimmed = trim_space(push_s);
    if (trimmed == "")   return;

    println("finding {} in map", trimmed);

    if (symbol_property.contains(trimmed)) // symbol
    {
        tmp.token_type = symbol_property[trimmed].enum_type;
    
    } 
    else
    {
        tmp.token_type = TEXT;
        tmp.text = trimmed;
    }
    token.push_back(tmp);
}

int tokenizer(string s)
{
    if (s.size() == 0) return -1;

    size_t len = s.size();
    size_t prev = 0;
    size_t token_vector_index = 0;
    string prev_str = "";
    bool is_in_symbol;
    int bracket_depth = 0;


    for (size_t i = 0; i < len; i++)
    {
        println("---");
        println("prev {} current char {}", prev, s[i]);

        prev_str += s[i];
        bracket_depth_changer(s[i], bracket_depth);

        if ((i+1 == len) || !token_continue(prev_str, s[i+1]))
        {
            token_push(prev_str, bracket_depth);
            prev_str = "";
            continue;
        }
    }
    return 0;
}

}
