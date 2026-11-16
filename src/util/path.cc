#include "util/path.h"

#include <glob.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "error.h"

using namespace std;
using namespace ov4;

namespace ov4::path
{

std::vector<file_property_t> fcv(file_property_t &property)
{
    std::vector<file_property_t> v;
    glob_t pglob;
    int ret = glob("/*", 0, nullptr, &pglob);
    if (ret != 0 || pglob.gl_pathc == 0)
    {
        throw T_error {error_code::UTIL_PATH_ERR, "UTIL_PATH_ERR"};
        return v;
    }
    

    for (int i = 0; pglob.gl_pathv[i] != nullptr; i++)
    {
        const char * const p = pglob.gl_pathv[i];
        file_property_t this_file = { .ret = p};

        struct stat buffer_stat;
        int stat_ret = stat(pglob.gl_pathv[i], &buffer_stat);
        if (stat_ret != 0) throw T_error {error_code::UTIL_PATH_ERR, "UTIL_PATH_ERR"};

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

}
