#include "command/builtin.h"

#include <cerrno>
#include <unordered_map>

using namespace std;

// constexpr string_view
//     err_EACCES = "cd: Search permission is denied for one of the components of path.",
//     err_EFAULT = "cd: path points outside your accessible address space.",
//     err_EIO = "An I/O error occurred.",
//     err_ENOTDIR = "A component of path is not a directory.",
//     err_ENOENT = "The directory specified in path does not exist.",
//     err_ENAMETOOLONG = "path is too long.",
//     err_ENOMEM = "Insufficient kernel memory was available.";

const unordered_map<int, string_view> err_list = {
    { EACCES, "cd: Search permission is denied for one of the components of path." },
    { EFAULT, "cd: path points outside your accessible address space." },
    { EIO, "An I/O error occurred." },
    { ENOTDIR, "A component of path is not a directory." },
    { ENOENT, "The directory specified in path does not exist." },
    { ENAMETOOLONG, "path is too long." },
    { ENOMEM, "Insufficient kernel memory was available."},
};

namespace ov4::command
{

void cd(char **argv)
{
    if (argv == nullptr) return;
    if (argv[1] == nullptr) 
    {
        cout << "cd: requires path" << endl;
        return;
    }
    int ret = ov4::set_current_dir(argv[1]);
    if (ret != 0)
    {
        if (err_list.find(errno) != err_list.end())
        {
            cout << "cd: " << err_list.at(errno) << endl;
            return;
        }
        cout << "cd: Error occured, code: " << errno << endl;
    }
}

}
