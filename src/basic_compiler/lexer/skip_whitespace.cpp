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
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
            continue;
        }
        if (c == '\'') { // comment until end of line
            skipToEOL();
            continue;
        }
        break;
    }
}

} // namespace gwbasic
