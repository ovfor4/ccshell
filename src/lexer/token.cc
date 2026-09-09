#include "lexer/token.h"

#include <string>
#include <iostream>
#include <print>

#include "lexer/lexer_class.h"
#include "lexer/enum_type.h"
#include "lexer/symbol_property.h"
#include "util/io.h"
#include "error.h"

using namespace std; // remove this

namespace ov4
{

string T_lexer::trim(const string &s, const string &trim_target) // trim_target = " \t"
{
    size_t b = s.find_first_not_of(trim_target);
    if (b == string::npos)
        return "";
    size_t e = s.find_last_not_of(trim_target);
    return s.substr(b, e - b + 1);
}


bool T_lexer::is_single_symbol(char c)
{
    string s = " ";
    s[0] = c;
    if (symbol_property.contains(s))
        return true;
    return false;
}

void T_lexer::bracket_depth_changer(char c, int &bracket_depth)
{
    if (c == '(')
    {
        bracket_depth++;
        loggerln("changed depth: {}", bracket_depth);
        return;
    }
    if (c == ')')
    {
        bracket_depth--;
        loggerln("changed depth: {}", bracket_depth);
        if (bracket_depth < 0)
        {
            cerr << "Ooooops bracket unmatched, quitting" << endl;
            exit(-1); // TODO: std::expected or what, rather than quitting
        }
        return;
    }
}


bool T_lexer::token_continue(const string &s, char next)
{
    loggerln("token_continue: receiving {} {}", s, next);
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
        loggerln("symbol not found in map");
        return false;
    }
}

// handle [prev, current)
void T_lexer::token_push(const string &push_s, int bracket_depth, bool force_text) // force_text = false
{
    T_token tmp;
    tmp.bracket_depth = bracket_depth;
    string trimmed = trim(push_s);
    if (trimmed == "")   return;

    loggerln("finding {} in map", trimmed);

    if (symbol_property.contains(trimmed) && !force_text) // symbol
    {
        loggerln("token_push: adding symbol");
        tmp.token_type = symbol_property[trimmed].enum_type;
    } 
    else
    {
        loggerln("token_push: adding text");
        if (force_text)
            trimmed = trim(trimmed, "\'\"");
        tmp.token_type = TEXT;
        tmp.text = trimmed;
    }
    token.push_back(tmp);
}

int T_lexer::tokenizer(const string &s)
{
    if (s.size() == 0) return -1;

    size_t len = s.size();
    size_t token_vector_index = 0;
    string prev_str = "";
    bool force_text = false;
    int bracket_depth = 0;
    guard_type guard = GUARD_OFF;


    for (size_t i = 0; i < len; i++)
    {

        char c = s[i];

        prev_str += c;
        
        loggerln("---");
        loggerln("prev_str .{}. next char {}", prev_str, s[i+1]);

        if (c == '\'' || c == '\"')
        {
            loggerln("quotation");

            // turn on GUARD
            if (guard == GUARD_OFF)
            {
                guard = (c == '\'') ? GUARD_STRONG : GUARD_WEAK;
                loggerln("set guard {}", (c == '\'') ? "GUARD_STRONG" : "GUARD_WEAK");
                continue;
            }
            // turn off
            else if (guard == GUARD_STRONG && c == '\'')
            {
                guard = GUARD_OFF;
                loggerln("quitting strong guard");
                force_text = true;
            }
            //turn off
            else if (guard == GUARD_WEAK && c == '\"')
            {
                guard = GUARD_OFF;
                loggerln("quitting weak guard");
                force_text = true;
            } else
                continue;
        }
        // not quotation mark
        else if ((guard == GUARD_STRONG || guard == GUARD_WEAK))
        {
            loggerln("tokenizer: guarded, continue");
            continue;
        }

        bracket_depth_changer(c, bracket_depth);

        if ((i+1 == len) || !token_continue(prev_str, s[i+1]))
        {
            token_push(prev_str, bracket_depth, force_text);
            force_text = false;
            prev_str = "";
            continue;
        }
    }
    return 0;
}

}
