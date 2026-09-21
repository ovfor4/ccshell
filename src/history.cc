#include "history.h"

using namespace std;

namespace ov4
{

void add_history(const std::string &s)
{
    history_vec.push_back(s);
}

void clear_history()
{
    history_vec.clear();
}
    
}
