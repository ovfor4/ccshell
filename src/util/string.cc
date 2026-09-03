#include "util/string.h"

using namespace std;

namespace ov4
{

void string2argv(const vector<string> * const s, char **argv)
{

    // pointers [0, size)
    // nullptr [size]
    // data  [size+1, ...)

    char *p = (char*)(argv + s->size() + 1);

    argv[s->size()] = nullptr;

    for (size_t i = 0; i < s->size(); i++)
    {
        strcpy(p, (*s)[i].c_str());
        argv[i] = p;
        p += (*s)[i].size() + 1; // for '\0'
    }
}

void argv2string(vector<string> *s, const char * const * argv)
{
    if (argv == nullptr) return; 
    
    for (size_t i = 0; true; i++)
    {
        if (argv[i] == nullptr) return;
        s->push_back(argv[i]);
    }
}

std::string safe_str(const char *s, const char *fallback)
{
    return (s != nullptr) ? s : fallback; 
}

}
