#pragma once

#include <vector>
#include <string>

#ifdef UTIL_DIR_VERBOSE
#include <util/io.h>
#define UTIL_DIR_LOGGERLN(fmt, ...) (ov4::loggerln(fmt __VA_OPT__(,) __VA_ARGS__),0)
#else
#define UTIL_DIR_LOGGERLN(fmt, ...) (0)
#endif


namespace ov4::dir
{

struct ls_t
{
    mode_t mode;
    std::string path;
};

/*
 * lsv: list directory contents
 */
std::vector<ls_t> lsv(const std::string &path, bool ignore_hidden, bool ignore_dot);

/*
 * appslash: append slash
 */
std::string appslash(const std::string &path);

}
