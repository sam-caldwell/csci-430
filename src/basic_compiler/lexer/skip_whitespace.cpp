// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: Lexer::skipWhitespace
 * Summary:
 *  Skip spaces, tabs, carriage returns, and apostrophe comments to EOL.
 * Parameters:
 *  - none
 * Returns:
 *  - void
 */
void Lexer::skipWhitespace() {
    while (!atEnd()) {
        skipWhile([](const char chr){ return chr == Symbols::SPACE.first() || chr == Symbols::TAB.first() || chr == Symbols::CR.first(); });
        if (peek() == Symbols::SINGLE_QUOTE.first()) { // comment until end of line
            skipToEOL();
            continue;
        }
        break;
    }
}

} // namespace gwbasic
