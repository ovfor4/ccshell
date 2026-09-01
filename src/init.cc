#include "init.h"

namespace ov4
{

using namespace std;

void init()
{
    cout << "\
You are using ccshell. \n\
This is free software; see the source for copying conditions. \
There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. \n\
Published under GNU AFFERO GENERAL PUBLIC LICENSE (AGPL) Version 3." << endl;

    signal_init();
    set_current_dir(get_current_dir_name());


}

}
