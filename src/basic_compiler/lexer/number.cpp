// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"

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
    bool seenDot = false;
    const auto buf = scanWhile([&](const char ch) {
        if (const auto uch = static_cast<unsigned char>(ch); std::isdigit(uch)) return true;
        if (!seenDot && ch == '.') { seenDot = true; return true; }
        return false;
    });
    if (seenDot) return Token{TokenType::Float, buf, startLine, startCol};
    return Token{TokenType::Integer, buf, startLine, startCol};
}

} // namespace gwbasic
