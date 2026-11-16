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
    ALL,
    TRUE,
    FALSE,
};

enum class path_mode_t
{
    RELATIVE,
    ABSOLUTE,
};

struct file_property_t
{
    executable_t executable = executable_t::ALL;
    mode_t file_type = 0; // all
    bool in_path_env = true;
    bool in_dir = true;
    std::string candidate_dir = "";

    path_mode_t path_mode = path_mode_t::RELATIVE;

    bool (*custom_check)(const char *) = nullptr;

    std::string ret = "";
};

// find candidate vector
std::vector<file_property_t> fcv(file_property_t &property);

}
