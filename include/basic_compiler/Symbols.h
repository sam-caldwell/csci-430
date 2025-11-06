// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/Symbol.h"

namespace gwbasic {

/**
 * Symbols: Centralized table of ASCII symbols used by the lexer/parser.
 *
 * Provides single- and double-byte operator/punctuation symbols as
 * inline constexpr instances of Symbol.
 */
struct Symbols {
    // Control/whitespace
    static inline constexpr Symbol SPACE        {' '};
    static inline constexpr Symbol TAB          {'\t'};
    static inline constexpr Symbol LF           {'\n'};
    static inline constexpr Symbol CR           {'\r'};
    static inline constexpr Symbol NUL          {'\0'};
    static inline constexpr Symbol DEL          {static_cast<char>(0x7F)};

    // Quotes and identifier markers
    static inline constexpr Symbol SINGLE_QUOTE {'\''};
    static inline constexpr Symbol DOUBLE_QUOTE {'"'};
    static inline constexpr Symbol DOLLARSIGN   {'$'};

    // Operators / punctuation (single)
    static inline constexpr Symbol PLUS         {'+'};
    static inline constexpr Symbol MINUS        {'-'};
    static inline constexpr Symbol STAR         {'*'};
    static inline constexpr Symbol SLASH        {'/'};
    static inline constexpr Symbol LPAREN       {'('};
    static inline constexpr Symbol RPAREN       {')'};
    static inline constexpr Symbol COLON        {':'};
    static inline constexpr Symbol COMMA        {','};
    static inline constexpr Symbol HASH         {'#'};
    static inline constexpr Symbol EQUALS       {'='};
    static inline constexpr Symbol LESS_THAN    {'<'};
    static inline constexpr Symbol GREATER_THAN {'>'};
    static inline constexpr Symbol AMPERSAND    {'&'};
    static inline constexpr Symbol PERCENT      {'%'};
    static inline constexpr Symbol EXCLAMATION  {'!'};
    static inline constexpr Symbol CARET        {'^'};

    // Operators (double)
    static inline constexpr Symbol LESS_EQUAL   {"<="};
    static inline constexpr Symbol GREATER_EQUAL{">="};
    static inline constexpr Symbol NOT_EQUAL    {"<>"};

    // Common 1-byte string fragments (for streaming convenience)
    // Note: kept for easy migration of existing STR_* uses.
    static inline constexpr char STR_LF[2]        = "\n";
    static inline constexpr char STR_DBL_QUOTE[2] = "\"";
    static inline constexpr char STR_SPACE[2]     = " ";
    static inline constexpr char STR_PERCENT[2]   = "%";
};

} // namespace gwbasic
