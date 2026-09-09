#pragma once

#include <stdexcept>

namespace ov4
{

enum class error_code
{

    // tokenizer
    TOKEN_UNMATCHED_BRACKET,

    // AST
    AST_MISS_LEFT_CHILD,
    AST_MISS_RIGHT_CHILD,
    AST_EXIST_LEFT_CHILLD,
    AST_EXIST_RIGHT_CHILD,

};

class T_error : public std::runtime_error {
public:
    error_code code;
    T_error(error_code k, std::string msg)
        : std::runtime_error(std::move(msg)), code(k) {}
};
    
}
