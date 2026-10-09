#include "history.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <print>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

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
    string tmp_path = path + ".tmp";
    int fd = open(tmp_path.c_str(), (O_WRONLY | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR));
    if (fd == -1) 
    {
        loggerln("save_history: fail to open() history");
        return;
    }
    FILE *f = fdopen(fd, "w");
    if (f == nullptr)
    {
        close(fd);
        loggerln("save_history: fail to fdopen() history");
        return;
    }

    for (auto &c : history_vec)
    {
        println(f, "{}", c);
    }


    bool success_write = true;
    if (fflush(f) != 0) success_write = false;
    if (fsync(fd) != 0) success_write = false;
    if (fclose(f) != 0) success_write = false;

    if (!success_write)
    {
        loggerln("save_history: fail to write");
        return;
    }

    // atomic rename (replace)
    if (rename(tmp_path.c_str(), path.c_str()) != 0)
    {
        loggerln("save_history: fail to rename");
        return;
    }
}
    
}
