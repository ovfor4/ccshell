#include "history.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <print>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include "util/io.h"

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

size_t get_last_index()
{
    return history_vec.size() - 1;
}

string get_default_history_path()
{
    return get_home_dir() + '/' + ".ccshell_history";
}

void load_history(const string &path)
{
    history_vec.clear();
    ifstream ifs(path);
    if (!ifs.is_open())
    {
        loggerln("load_history: fail to open history");
        return;
    }
    string tmp;
    while (getline(ifs, tmp))
    {
        history_vec.push_back(tmp);
    }
    return;
}

void save_history(const string &path)
{
    int fd = open(path.c_str(), O_WRONLY | O_CREAT);
    if (fd == -1) 
    {
        loggerln("save_history: fail to open() history");
        return;
    }
    char f_mode = 'w';
    FILE *f = fdopen(fd, &f_mode);
    if (f == nullptr)
    {
        loggerln("save_history: fail to fdopen() history");
        close(fd);
        return;
    }

    for (auto &c : history_vec)
    {
        println(f, "{}", c);
    }

    fflush(f);
    fsync(fd);
    fclose(f);
}
    
}
