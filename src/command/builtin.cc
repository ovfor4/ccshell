#include "command/builtin.h"

namespace ov4
{

using namespace std;

//array<string> builtin_cmd_list{"quit", "fg", "bg", "jobs"};

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
bool exe_bultin_command(char **argv) 
{
    if (argv == nullptr || argv[0] == nullptr) exit(1); // WTF???
    if (strcmp(argv[0], "quit") == 0) { ov4::command::quit(); return true; } 
    else return false;
}



}
