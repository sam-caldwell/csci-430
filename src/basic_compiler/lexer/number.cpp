// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/lexer/LexError.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: Lexer::number
 * Summary:
 *  Scan a numeric literal and return Integer or Float token.
 * Parameters:
 *  - none
 * Returns:
 *  - Token: Integer or Float token with lexeme and location
 */
Token Lexer::number() { // NOLINT(readability-function-cognitive-complexity,readability-function-size)
    const int startLine = line_;
    const int startCol = col_;
    std::string buf;
    bool seenDot = false;
    bool seenExp = false;

    // integer part
    while (!atEnd() && (std::isdigit(static_cast<unsigned char>(peek())) != 0)) {
        buf.push_back(advance());
    }
    // fractional part
    if (!atEnd() && peek() == '.') {
        seenDot = true;
        buf.push_back(advance());
        while (!atEnd() && (std::isdigit(static_cast<unsigned char>(peek())) != 0)) {
            buf.push_back(advance());
        }
    }
    // exponent part: E or D with optional sign and digits
    if (!atEnd()) {
        const unsigned char cur = static_cast<unsigned char>(peek());
        if (const char upper = static_cast<char>(std::toupper(cur)); upper == 'E' || upper == 'D') {
            seenExp = true;
            // Normalize 'D' to 'E' to allow standard parsing downstream
            buf.push_back('E');
            advance();
            if (!atEnd() && (peek() == '+' || peek() == '-')) {
                buf.push_back(advance());
            }
            // require at least one digit in exponent
            int expDigits = 0;
            while (!atEnd() && (std::isdigit(static_cast<unsigned char>(peek())) != 0)) {
                buf.push_back(advance());
                ++expDigits;
            }
            if (expDigits == 0) {
                std::ostringstream oss; oss << "Invalid exponent at " << startLine << ':' << startCol;
                throw LexError(oss.str());
            }
        }
    }

    if (seenDot || seenExp) {
        return Token{TokenType::Float, buf, startLine, startCol};
    }
    return Token{TokenType::Integer, buf, startLine, startCol};
}

} // namespace gwbasic
