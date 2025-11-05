// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"

namespace gwbasic {

/*
 * Function: Lexer::skipWhitespace
 * Inputs:
 *  - none (operates on internal stream state)
 * Outputs:
 *  - void
 * Theory of operation:
 *  - Advances past spaces, tabs and carriage returns. If an apostrophe is
 *    encountered, treats the rest of the line as a comment and skips to EOL.
 */
void Lexer::skipWhitespace() {
    while (!atEnd()) {
        skipWhile([](const char c){ return c == Symbols::SPACE.first() || c == Symbols::TAB.first() || c == Symbols::CR.first(); });
        if (peek() == Symbols::SINGLE_QUOTE.first()) { // comment until end of line
            skipToEOL();
            continue;
        }
        break;
    }
}

} // namespace gwbasic
