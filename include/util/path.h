#pragma once

#include <string>
#include <vector>

#ifdef UTIL_PATH_VERBOSE
#include <util/io.h>
#define UTIL_PATH_LOGGERLN(fmt, ...) (ov4::loggerln(fmt __VA_OPT__(,) __VA_ARGS__),0)
#else
#define UTIL_PATH_LOGGERLN(fmt, ...) (0)
#endif

namespace ov4::path
{

enum class executable_t
{
    ALL, /* accept executable or non-executable file */
    TRUE, /* executable only */
    FALSE, /* non-executable only */
};

enum class path_mode_t
{
    RELATIVE, /* relative to current working dir */
    ABSOLUTE, /* absolute path starting with <slash> */
};

enum class dir_type_t
{
    PATH,
    CUSTOM,
};

struct file_property_t
{
    /* intended executability */
    executable_t executable = executable_t::ALL;

    /* see stat(2) */
    mode_t file_type = 0; /* all */

    /* 
     * whether find in path 
     * if true, path files would be ABSOLUTE
     * and cwd files would keep RELATIVE
     */
    dir_type_t dir_type = dir_type_t::PATH;

    /* absolute or relative dir */
    std::string pattern = "";

    /* ABSOLUTE or RELATIVE path */
    path_mode_t path_mode = path_mode_t::RELATIVE;

    /*
     * custom property checker
     * if matches, it should return true
     */
    bool (*custom_check)(const char * _path) = nullptr;

    /*
     * return value
     * path of candidate
     */
    std::string ret = "";
};

/*
 * fcv: find candidate vector
 * find files based on file_property_t conditions
 * returns valid cadidates with their properties filled in file_property_t
 */
std::vector<file_property_t> fcv(const file_property_t &property);

/*
 * split: split path
 */
std::vector<std::string> split(std::string &s);

}
