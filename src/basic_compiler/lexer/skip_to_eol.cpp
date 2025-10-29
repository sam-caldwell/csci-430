// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"

namespace gwbasic {

/*
 * Function: Lexer::skipToEOL
 * Inputs:
 *  - none
 * Outputs:
 *  - void
 * Theory of operation:
 *  - Consumes characters until a newline or end-of-input is reached.
 */
void Lexer::skipToEOL() {
    while (!atEnd() && peek() != '\n') advance();
}

} // namespace gwbasic
