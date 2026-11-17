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
    std::vector<std::string> pattern_v;
    std::vector<std::string> res;

    /* set intented dir */
    if (property.dir_type == dir_type_t::PATH)
    {
        /* PATH can be splitted into some dir's */
        const char *sp = getenv("PATH");
        if (sp == nullptr)
            return v;
        
        string s = sp;
        auto ret_split = split(s);
        for (auto &t : ret_split)
        {
            file_property_t tmp = property;
            tmp.pattern = t + '/' + property.pattern;
            pattern_v.push_back(tmp.pattern);
        }
    }
    else
        pattern_v.push_back(property.pattern);


    for (auto &t : pattern_v)
    {
        /* get an array of qualified files */
        __attribute__((cleanup(globfree))) glob_t g = {};
        int ret = glob(t.c_str(), 0, nullptr, &g);
        if (ret != 0 || g.gl_pathc == 0)
        {
            continue;
        }

        /* copy results to vector */
        for (int i = 0; g.gl_pathv[i] != nullptr; i++)
        {
            res.push_back(g.gl_pathv[i]);
        }
    }
    

    for (auto &t : res)
    {
        const char * const p = t.c_str();
        file_property_t this_file = { .ret = p};

        struct stat buffer_stat;
        int stat_ret = stat(p, &buffer_stat);
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
    std::vector<std::string> v;
    size_t cur = 0, prev = 0;
    while (true)
    {
        cur = s.find(':', prev);
        if (cur == string::npos)
            cur = s.size();

        string tmp = s.substr(prev, cur-prev);
        v.push_back(tmp);
        prev = cur+1;
        if (cur == s.size())
            break;
    }
    return v;
}

}
