// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: Lexer::peek
 * Summary:
 *  Inspect the current character without consuming it.
 * Parameters:
 *  - none
 * Returns:
 *  - char: current character or NUL at end-of-input
 */
char Lexer::peek() const { return atEnd() ? Symbols::NUL.first() : src_[pos_]; }

} // namespace gwbasic

