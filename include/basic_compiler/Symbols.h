// NOLINTBEGIN(llvm-header-guard)
#ifndef BASIC_COMPILER_SYMBOLS_H
#define BASIC_COMPILER_SYMBOLS_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

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
    static constexpr Symbol SPACE        {' '};
    static constexpr Symbol TAB          {'\t'};
    static constexpr Symbol LF           {'\n'}; // NOLINT(readability-identifier-length)
    static constexpr Symbol CR           {'\r'}; // NOLINT(readability-identifier-length)
    static constexpr Symbol NUL          {'\0'};
    static constexpr Symbol DEL          {static_cast<char>(0x7F)};

    // Quotes and identifier markers
    static constexpr Symbol SINGLE_QUOTE {'\''};
    static constexpr Symbol DOUBLE_QUOTE {'"'};
    static constexpr Symbol DOLLARSIGN   {'$'};

    // Operators / punctuation (single)
    static constexpr Symbol PLUS         {'+'};
    static constexpr Symbol MINUS        {'-'};
    static constexpr Symbol STAR         {'*'};
    static constexpr Symbol SLASH        {'/'};
    static constexpr Symbol LPAREN       {'('};
    static constexpr Symbol RPAREN       {')'};
    static constexpr Symbol COLON        {':'};
    static constexpr Symbol SEMICOLON    {';'};
    static constexpr Symbol COMMA        {','};
    static constexpr Symbol HASH         {'#'};
    static constexpr Symbol EQUALS       {'='};
    static constexpr Symbol LESS_THAN    {'<'};
    static constexpr Symbol GREATER_THAN {'>'};
    static constexpr Symbol AMPERSAND    {'&'};
    static constexpr Symbol PERCENT      {'%'};
    static constexpr Symbol EXCLAMATION  {'!'};
    static constexpr Symbol CARET        {'^'};

    // Operators (double)
    static constexpr Symbol LESS_EQUAL   {"<="};
    static constexpr Symbol GREATER_EQUAL{">="};
    static constexpr Symbol NOT_EQUAL    {"<>"};

    // Common 1-byte string fragments (for streaming convenience)
    // (Removed legacy STR_* char arrays; prefer Symbol or string literals.)
};

} // namespace gwbasic

#endif // BASIC_COMPILER_SYMBOLS_H
// NOLINTEND(llvm-header-guard)
