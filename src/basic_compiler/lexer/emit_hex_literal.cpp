// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <sstream>
#include <cctype>

namespace gwbasic {

/*
 * Function: Lexer::emitHexLiteral
 * Purpose:
 *  - Consume an '&' followed by 'H'/'h' and a sequence of hex digits,
 *    emitting an Integer token with the parsed decimal value.
 * Inputs:
 *  - line/col: token position metadata captured by the caller
 * Throws:
 *  - LexError when the format is invalid (missing 'H' or digits)
 */
void Lexer::emitHexLiteral(std::vector<Token>& out, const int line, const int col) {
    // Caller should have peek() == '&'
    advance();
    const unsigned char cu = static_cast<unsigned char>(peek());
    const char n = static_cast<char>(std::toupper(cu));
    if (n != 'H') {
        std::ostringstream oss; oss << "Unexpected '&' at " << line << ':' << col;
        throw LexError(oss.str());
    }
    advance();

    unsigned long long val = 0ULL;
    int digits = 0;
    while (true) {
        const unsigned char ch = static_cast<unsigned char>(peek());
        int v;
        if (ch >= '0' && ch <= '9') v = ch - '0';
        else if (ch >= 'A' && ch <= 'F') v = 10 + (ch - 'A');
        else if (ch >= 'a' && ch <= 'f') v = 10 + (ch - 'a');
        else break;
        val = (val << 4) + static_cast<unsigned long long>(v);
        advance();
        ++digits;
    }
    if (digits == 0) {
        std::ostringstream oss; oss << "Invalid hex literal at " << line << ':' << col;
        throw LexError(oss.str());
    }
    Token t(TokenType::Integer, std::to_string(val), line, col);
    emitToken(out, t);
    bol_ = false;
}

} // namespace gwbasic

