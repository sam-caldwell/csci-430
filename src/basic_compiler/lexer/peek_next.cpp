// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: Lexer::peekNext
 * Summary:
 *  Look one character ahead without consuming.
 * Parameters:
 *  - none
 * Returns:
 *  - char: next character or NUL at end-of-input
 */
char Lexer::peekNext() const {
    return (pos_ + 1U < src_.size()) ? src_[pos_ + 1U] : Symbols::NUL.first();
}

} // namespace gwbasic

