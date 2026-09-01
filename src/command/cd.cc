#include "command/builtin.h"

namespace ov4::command
{

using namespace std;

void cd(char **argv)
{
    if (argv == nullptr) return;
    if (argv[1] == nullptr) 
    {
        cout << "cd: requires path" << endl;
        return;
    }
    ov4::set_current_dir(argv[1]);
}

}
