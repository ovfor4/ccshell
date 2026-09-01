#include "init.h"

#include <fcntl.h>

using namespace std;

namespace ov4
{

void init()
{
    cout << "\
You are using ccshell. \n\
This is free software; see the source for copying conditions. \
There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. \n\
Published under GNU AFFERO GENERAL PUBLIC LICENSE (AGPL) Version 3." << endl;

    signal_init();

    tty_fd = open("/dev/tty", O_RDWR);

}

}
