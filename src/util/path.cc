#include "util/path.h"

#include <glob.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "error.h"
#include "util/string.h"

using namespace std;
using namespace ov4;

namespace ov4::path
{

std::vector<file_property_t> fcv(const file_property_t &property)
{
    std::vector<file_property_t> v;
    __attribute__((cleanup(globfree))) glob_t pglob = {};
    string s;

    /* set intented dir */
    if (property.dir_type == dir_type_t::PATH)
    {
        const char *sp = getenv("PATH");
        if (sp == nullptr)
            return v;
        s = sp + '/' + property.pattern;
    }
    else
        s = property.pattern;

    UTIL_PATH_LOGGERLN("111");
    int ret = glob(s.c_str(), 0, nullptr, &pglob);
    UTIL_PATH_LOGGERLN("ret {} {}", ret, GLOB_NOMATCH);
    if (ret != 0 || pglob.gl_pathc == 0)
    {
        return v;
    }
    
    UTIL_PATH_LOGGERLN("search");
    for (int i = 0; pglob.gl_pathv[i] != nullptr; i++)
    {
        const char * const p = pglob.gl_pathv[i];
        file_property_t this_file = { .ret = p};

        struct stat buffer_stat;
        int stat_ret = stat(pglob.gl_pathv[i], &buffer_stat);
        if (stat_ret != 0) continue;

        // check executable
        int executable_ret = access(p, X_OK);
        this_file.executable = (executable_ret == 0) ? (executable_t::TRUE) : (executable_t::FALSE);

        if ((property.executable != executable_t::ALL)
            && (property.executable != this_file.executable))
            continue;

        
        // check file type
        this_file.file_type = buffer_stat.st_mode;
        if ((property.file_type != 0)
            && (property.file_type != this_file.file_type))
            continue;

        // custom check
        bool (*custom_check)(const char *) = property.custom_check;
        if ((custom_check != nullptr)
            && !((*custom_check)(p)))
            continue;

        v.push_back(this_file);
        
    }

    return v;
}

std::vector<std::string> split(std::string &s)
{
    UTIL_PATH_LOGGERLN("{}", s.size());
    std::vector<std::string> v;
    size_t cur = 0, prev = 0;
    while (true)
    {
        sleep(1);
        cur = s.find(':', prev);
        if (cur == string::npos)
            cur = s.size();

        string tmp = s.substr(prev, cur-prev);
        UTIL_PATH_LOGGERLN("{} {}", prev, cur);
        UTIL_PATH_LOGGERLN("{}", tmp);
        v.push_back(tmp);
        prev = cur+1;
        if (cur == s.size())
            break;
    }
    return v;
}

}
