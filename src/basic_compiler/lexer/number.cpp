// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <cctype>

namespace gwbasic {

/*
 * Function: Lexer::number
 * Inputs:
 *  - none (reads from current source position)
 * Outputs:
 *  - Token: Integer or Float token with lexeme and source location
 * Theory of operation:
 *  - Scans digits and a single optional decimal point to form a numeric
 *    literal; classifies as float if a dot was seen.
 */
Token Lexer::number() {
    const int startLine = line_;
    const int startCol = col_;
    std::string buf;
    bool seenDot = false;
    while (std::isdigit(peek()) || (!seenDot && peek() == '.')) {
        if (peek() == '.') seenDot = true;
        buf.push_back(advance());
    }
    if (seenDot) return Token{TokenType::Float, buf, startLine, startCol};
    return Token{TokenType::Integer, buf, startLine, startCol};
}

} // namespace gwbasic
