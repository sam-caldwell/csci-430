// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: Lexer::advance
 * Summary:
 *  Consume one character and update source position state.
 * Parameters:
 *  - none
 * Returns:
 *  - char: the character consumed, or '\0' at end-of-input
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
