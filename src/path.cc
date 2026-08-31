#include "path.h"

namespace ov4
{

using namespace std;

string find_cmd(char* s)
{
    string tmp = s;
    return find_cmd(tmp);
}

string find_cmd(string s)
{
    if (s.size() == 0) return "/";
    if (s[0] == '/') return s;

    string path = getenv("PATH");
    size_t i = 0, j = 0, l = path.size();
    while (true)
    {
        for (; j < l; j++)
            if (path[j] == ':')
                break;
        
        string cmd = path.substr(i, j-i) + '/' +s;
        cout << cmd << endl;
        if (access(cmd.c_str(), F_OK) == 0)
            return cmd;

        if (j == l)
            return "/";

        j++;
        i=j;

    }
}


}
