#include "util/option_parse.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <utility>

#include "all.h"

using namespace std;

const char option_key_value_split = '=';
constexpr char option_prefix = '-';

namespace ov4
{

void exe_option(char **c_argv)
{
    vector<string> argv;
    argv2string(&argv, c_argv);

    for (auto c : argv)
    {
        pair<string, string> key_value = option_split(c);
        if (key_value.first != "")
        {
            if (option_list.find(key_value.first) == option_list.end())
            {
                cerr << key_value.first << ": option not fund" << endl;
                continue;
            }
            option_list.at(key_value.first)(key_value.second);
        }
    }
}

pair<string, string> option_split(string s)
{
    if (s.size() == 0) return make_pair("", "");
    if (s[0] != option_prefix) return make_pair("", "");
    size_t pos = s.find(option_key_value_split);
    if (pos == string::npos) return make_pair(s, "");
    if (pos == s.size()+1) return make_pair(s.substr(0, pos), "");
    string key = s.substr(0, pos);
    string value = s.substr(pos+1);
    return make_pair(key, value);
}

}
