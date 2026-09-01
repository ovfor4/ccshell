#include "command/builtin.h"

namespace ov4
{

using namespace std;

//array<string> builtin_cmd_list{"quit", "fg", "bg", "jobs"};

unordered_map<string, function<void(char**)>> cmd =
{
    { "quit", ov4::command::quit },
    { "exit", ov4::command::exit },
    { "jobs", ov4::command::jobs },
    { "bg", ov4::command::bg },
    { "fg", ov4::command::fg },
    { "cd", ov4::command::cd },
};

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
bool exe_bultin_command(char **argv) 
{
    if (argv == nullptr || argv[0] == nullptr) return false;
    string s = argv[0];
    if (cmd.find(s) != cmd.end())
    {
        cmd[s](argv);
        return true;
    }
    return false;
}



}
