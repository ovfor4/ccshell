#include "util/string.h"

#include <print>

using namespace std;

namespace ov4
{

/*
 * construct C-style **argv
 * user should be responsible for allocation of **argv
 * and make sure it's large enough
 * argv should be the address to an empty and continuous memory space
 */
void string2argv(const vector<string> &v, char **argv)
{

    // pointers [0, size)
    // nullptr [size]
    // data  [size+1, ...)

    char *p = (char*)(argv + v.size() + 1);

    argv[v.size()] = nullptr;

    for (size_t i = 0; i < v.size(); i++)
    {
        strcpy(p, v[i].c_str());
        argv[i] = p;
        p += v[i].size() + 1; // for '\0'
    }
}

/*
 * allocate heap memory, and construct C-style **argv
 * NB: user should be responsible to ::operator delete();
 */
char **string2argv_alloc(const std::vector<std::string> &v)
{
    size_t size = argv_get_required_size(v);
    char **p = static_cast<char**>(operator new(size));
    string2argv(v, p);
    return p;
}

void argv2string(vector<string> &v, const char * const * argv)
{
    if (argv == nullptr) return; 

    v.clear();
    
    for (size_t i = 0; true; i++)
    {
        if (argv[i] == nullptr) return;
        v.push_back(argv[i]);
    }
}

/*
 * return the memory size required to construct argv[][]
 * in byte
 */
size_t argv_get_required_size(const std::vector<std::string> &v)
{
    size_t ret = 0; 
    ret += sizeof(char*) * (v.size() + 1); // point to every strings in v, and nullptr terminator
    for (auto &sub : v)
        ret += sizeof(char) * (sub.size() + 1); // chars, and null terminator
    return ret;
}

std::string safe_str(const char *s, const char *fallback)
{
    return (s != nullptr) ? s : fallback; 
}

void print_str_hex(const std::string &s)
{
    for (unsigned char c : s)
    {
        print("{:02X} ", c);
    }
}

}
