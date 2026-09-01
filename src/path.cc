#include "path.h"

namespace ov4
{

using namespace std;

string find_cmd(const char * s)
{
    const string tmp = s;
    return find_cmd(tmp);
}

string find_cmd(const string &s)
{
    if (s.size() == 0) return "";
    string unified_s = path_unifier(s);
    if (access(unified_s.c_str(), F_OK) == 0)
            return unified_s;

    string path = getenv("PATH");
    size_t i = 0, j = 0, l = path.size();
    while (true)
    {
        for (; j < l; j++)
            if (path[j] == ':')
                break;
        
        string cmd = path.substr(i, j-i) + '/' +s;
        if (access(cmd.c_str(), F_OK) == 0)
            return cmd;

        if (j == l)
            return "/";

        j++;
        i=j;

    }
}

string path_unifier(const string &s, bool is_cmd)
{
    if (s.size() == 0) return "";
    if (s[0] == '/') return s;
    if (s[0] == '.')
    {
        return get_current_dir() + s.substr(1);
    }
}

int set_current_dir(const string &s)
{
    return chdir(s.c_str());
}

string get_current_dir()
{
    return (string)(get_current_dir_name());
}


}
