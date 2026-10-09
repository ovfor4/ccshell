#pragma once

#include <string>
#include <vector>

#include "path.h"

namespace ov4
{

inline std::vector<std::string> history_vec;

inline size_t history_index;

void add_history(const std::string &s);
void clear_history();

std::size_t get_last_index();

std::string get_default_history_path();

void load_history(const std::string &path = get_default_history_path());
void save_history(const std::string &path = get_default_history_path());

}
