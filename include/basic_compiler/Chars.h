// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

namespace gwbasic {

// Single-character ASCII constants
inline constexpr char CH_SPACE         = 0x20;   // ' '
inline constexpr char CH_DEL           = 0x7F;   // DEL
inline constexpr char CH_LF            = '\n';   // Line feed
inline constexpr char CH_TAB           = '\t';   // Horizontal tab
inline constexpr char CH_CR            = '\r';   // Carriage return
inline constexpr char CH_SINGLE_QUOTE  = '\'';   // "'"
inline constexpr char CH_NULL          = '\0';   // NUL
inline constexpr char CH_DOLLARSIGN    = '$';     // '$'

// Symbol aliases for operators/punctuation used by the lexer
inline constexpr char SYM_PLUS         = '+';
inline constexpr char SYM_MINUS        = '-';
inline constexpr char SYM_STAR         = '*';
inline constexpr char SYM_SLASH        = '/';
inline constexpr char SYM_LPAREN       = '(';
inline constexpr char SYM_RPAREN       = ')';
inline constexpr char SYM_COLON        = ':';
inline constexpr char SYM_COMMA        = ',';
inline constexpr char SYM_HASH         = '#';
inline constexpr char SYM_EQUALS       = '=';
inline constexpr char SYM_LESS_THAN    = '<';
inline constexpr char SYM_GREATER_THAN = '>';
inline constexpr char SYM_AMPERSAND    = '&';
inline constexpr char SYM_DBL_QUOTE    = '"';

// Common string fragments
inline constexpr char STR_LF[]         = "\n";
inline constexpr char STR_DBL_QUOTE[]  = "\"";
inline constexpr char STR_SPACE[]      = " ";
inline constexpr char STR_PERCENT[]    = "%";

} // namespace gwbasic
