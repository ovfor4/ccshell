#pragma once

#include <string>
#include <vector>

#include "lexer/enum_type.h"

namespace ov4
{

class T_ast;
class T_token;

inline size_t ast_index = 0;

inline std::vector<T_ast> ast;
inline std::vector<T_token> token;

}
