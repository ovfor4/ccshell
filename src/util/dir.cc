#include "util/dir.h"

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

namespace ov4::dir
{

std::vector<ls_t> lsv(const std::string &path, bool ignore_hidden, bool ignore_dot)
{
    std::vector<ls_t> v;

    DIR *dp;
    if ((dp = opendir(path.c_str())) == nullptr)
    {
        return v;
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != nullptr)
    {
        if (entry->d_name == nullptr) continue;
        string s = entry->d_name;
        if (s.size() == 0) continue;

        /* UTIL_DIR_LOGGERLN("{}", s); */

        if (ignore_dot
            && (s == "." || s == ".."))
            continue;
        
        if (ignore_hidden && s[0] == '.')
            continue;

        s = appslash(path) + s;

        struct stat buf;
        int ret = stat(s.c_str(), &buf);
        if (ret != 0) continue;
        ls_t tmp = {.mode = buf.st_mode, .path = s};

        v.push_back(tmp);
    }

    return v;
    
}

std::string appslash(const std::string &path)
{
    if (path.size() == 0) return path;
    if (path[path.size()-1] == '/') return path;
    return (path + '/');
}

}
