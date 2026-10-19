#include "signal_handler.h"

namespace ov4
{

bool check_signal_delivered(int sig)
{
    return signal_mask[sig];
}

void set_signal_delivered(int sig)
{
    signal_mask[sig] = true;
}

void unset_signal_delivered(int sig)
{
    signal_mask[sig] = false;
}


}