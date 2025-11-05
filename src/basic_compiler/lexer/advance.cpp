// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"

namespace gwbasic {

/*
 * Function: Lexer::advance
 * Inputs:
 *  - none (uses internal source position state)
 * Outputs:
 *  - char: the character consumed, or '\0' at end-of-input
 * Theory of operation:
 *  - Moves the cursor forward one character, updating line/column counters
 *    and beginning-of-line flag when encountering a newline.
 */
char Lexer::advance() {
    if (atEnd()) return Symbols::NUL.first();
    const char c = src_[pos_++];
    if (c == Symbols::LF.first()) {
        line_++;
        col_ = 1;
        bol_ = true;
    } else {
        col_++;
    }
    return c;
}

} // namespace gwbasic
