// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: Lexer::number
 * Inputs:
 *  - none (reads from current source position)
 * Outputs:
 *  - Token: Integer or Float token with lexeme and source location
 * Theory of operation:
 *  - Scans digits with optional decimal point and optional scientific
 *    exponent (E or D with optional sign). Classifies as Float when a
 *    dot or exponent is present; otherwise Integer.
 */
Token Lexer::number() {
    const int startLine = line_;
    const int startCol = col_;
    std::string buf;
    bool seenDot = false;
    bool seenExp = false;

    // integer part
    while (!atEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        buf.push_back(advance());
    }
    // fractional part
    if (!atEnd() && peek() == '.') {
        seenDot = true;
        buf.push_back(advance());
        while (!atEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
            buf.push_back(advance());
        }
    }
    // exponent part: E or D with optional sign and digits
    if (!atEnd()) {
        const unsigned char cu = static_cast<unsigned char>(peek());
        char up = static_cast<char>(std::toupper(cu));
        if (up == 'E' || up == 'D') {
            seenExp = true;
            // Normalize 'D' to 'E' to allow standard parsing downstream
            buf.push_back('E');
            advance();
            if (!atEnd() && (peek() == '+' || peek() == '-')) {
                buf.push_back(advance());
            }
            // require at least one digit in exponent
            int expDigits = 0;
            while (!atEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
                buf.push_back(advance());
                ++expDigits;
            }
            if (expDigits == 0) {
                std::ostringstream oss; oss << "Invalid exponent at " << startLine << ':' << startCol;
                throw LexError(oss.str());
            }
        }
    }

    if (seenDot || seenExp) return Token{TokenType::Float, buf, startLine, startCol};
    return Token{TokenType::Integer, buf, startLine, startCol};
}

} // namespace gwbasic
