// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include "basic_compiler/LexError.h"
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace gwbasic {

/*
 * Function: Lexer::emitAmpLiteral
 * Summary:
 *  Parse an '&' numeric literal (&H, &O, &B, or octal shorthand) and emit it.
 * Parameters:
 *  - out: Token vector to append the emitted Integer token to
 *  - line: Source line for the token's starting position
 *  - col: Source column for the token's starting position
 * Returns:
 *  - void
 * Throws:
 *  - LexError when the literal format is invalid
 */
void Lexer::emitAmpLiteral(std::vector<Token>& out, const int line, const int col) {
    // Caller should have peek() == '&'
    advance();
    if (atEnd()) {
        std::ostringstream oss;
        oss << "Unexpected '&' at " << line << ':' << col;
        throw LexError(oss.str());
    }
    const unsigned char cu = static_cast<unsigned char>(peek());
    const char up = static_cast<char>(std::toupper(cu));

    unsigned long long val = 0ULL;
    int base = 0;

    if (up == 'H') { base = 16; advance(); }
    else if (up == 'O') { base = 8; advance(); }
    else if (up == 'B') { base = 2; advance(); }
    else if (std::isdigit(cu)) { base = 8; /* shorthand: &<octal> */ }
    else {
        std::ostringstream oss; oss << "Unexpected '&' at " << line << ':' << col; throw LexError(oss.str());
    }

    int digits = 0;
    while (!atEnd()) {
        const unsigned char ch = static_cast<unsigned char>(peek());
        int v = -1;
        if (base == 16) {
            if (ch >= '0' && ch <= '9') v = ch - '0';
            else if (ch >= 'A' && ch <= 'F') v = 10 + (ch - 'A');
            else if (ch >= 'a' && ch <= 'f') v = 10 + (ch - 'a');
            else break;
        } else if (base == 8) {
            if (ch >= '0' && ch <= '7') v = ch - '0';
            else break;
        } else if (base == 2) {
            if (ch == '0' || ch == '1') v = ch - '0';
            else break;
        }
        val = val * static_cast<unsigned long long>(base) + static_cast<unsigned long long>(v);
        advance();
        ++digits;
    }
    if (digits == 0) {
        std::ostringstream oss; oss << "Invalid & literal at " << line << ':' << col; throw LexError(oss.str());
    }
    Token t(TokenType::Integer, std::to_string(val), line, col);
    emitToken(out, t);
    bol_ = false;
}

} // namespace gwbasic
