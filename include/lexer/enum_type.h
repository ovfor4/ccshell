#pragma once

namespace ov4
{

enum enum_token_type
{
    TEXT,                   //    foo
    SINGLE_QUOTATION,       //    '
    DOUBLE_QUOTATION,       //    "
    LOGIC_AND,              //    &&
    LOGIC_OR,               //    ||
    ASYNC,                  //    &
    PIPE,                   //    |    
    SEMICOLON,              //    ;
    EXPAND,                 //    $
    LEFT_BRACKET,           //    (
    RIGHT_BRACKET,          //    )
};

enum guard_type
{
    GUARD_OFF,
    GUARD_WEAK,
    GUARD_STRONG,
};

}
