// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: Lexer::skipToEOL
 * Summary:
 *  Consume characters until a newline or end-of-input is reached.
 * Parameters:
 *  - none
 * Returns:
 *  - void
 */
void Lexer::skipToEOL() {
    skipWhile([](const char chr){ return chr != Symbols::LF.first(); });
}

} // namespace gwbasic
