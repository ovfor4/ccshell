#include "history.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <print>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <optional>

#include "util/io.h"
#include "util/file.h"

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

/*
 * load history from file
 */
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


/*
 * atomically save history to file
 */
void save_history(const string &path)
{
    /*
     * C++ and C standard lib do not guarantee that write is atomic
     * So if the program crashes, or computer shutdowns, it will leave a bad file
     * Then reading it may lead to problems if we launch the shell again
     * But unix guarantees rename(2) is atomic
     * So we just write to a temp file, and rename it to replace the "real" history file
     */
    string tmp_path = path + ".tmp";
    // int fd = open(tmp_path.c_str(), (O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW), (S_IRUSR | S_IWUSR));

    optional<T_unix_file_p> file_fp_optional;
    try
    {
        file_fp_optional.emplace(tmp_path.c_str(), (O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW), (S_IRUSR | S_IWUSR));
    }
    catch(const T_error &e)
    {
        std::cerr << e.what() << endl;
        switch (e.code)
        {
            case error_code::FILE_FAIL_TO_OPEN:
                loggerln("save_history: fail to open() history");
                break;

            case error_code::FILE_POINTER_FAIL_TO_OPEN:
                loggerln("save_history: fail to fdopen() history");

            default:
                ;
        }
        return;
    }

    T_unix_file_p &file_fp = *file_fp_optional;

    for (auto &c : history_vec)
    {
        println(file_fp.fp, "{}", c);
    }


    bool success_write = true;
    if (fflush(file_fp.fp) != 0) success_write = false;
    if (fsync(file_fp.fd) != 0) success_write = false;
    if (file_fp.close() != 0) success_write = false;

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
