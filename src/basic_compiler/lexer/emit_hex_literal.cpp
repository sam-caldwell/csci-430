// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/lexer/LexError.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
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
void Lexer::emitAmpLiteral(std::vector<Token>& out, const int line, const int col) { // NOLINT(readability-function-cognitive-complexity,readability-function-size)
    constexpr int BaseHex = 16;
    constexpr int BaseOct = 8;
    constexpr int BaseBin = 2;
    constexpr int HexDigitOffset = 10;
    // Caller should have peek() == '&'
    advance();
    if (atEnd()) {
        std::ostringstream oss;
        oss << "Unexpected '&' at " << line << ':' << col;
        throw LexError(oss.str());
    }
    const unsigned char cur = static_cast<unsigned char>(peek());
    const char upper = static_cast<char>(std::toupper(cur));

    unsigned long long val = 0ULL;
    int base = 0;

    if (upper == 'H') {
        base = BaseHex;
        advance();
    } else if (upper == 'O') {
        base = BaseOct;
        advance();
    } else if (upper == 'B') {
        base = BaseBin;
        advance();
    } else if (std::isdigit(static_cast<int>(cur)) != 0) {
        base = BaseOct; // shorthand: &<octal>
    } else {
        std::ostringstream oss;
        oss << "Unexpected '&' at " << line << ':' << col;
        throw LexError(oss.str());
    }

    int digits = 0;
    while (!atEnd()) {
        const unsigned char chr = static_cast<unsigned char>(peek());
        int value = -1;
        if (base == BaseHex) {
            if (chr >= '0' && chr <= '9') {
                value = chr - '0';
            } else if (chr >= 'A' && chr <= 'F') {
                value = HexDigitOffset + (chr - 'A');
            } else if (chr >= 'a' && chr <= 'f') {
                value = HexDigitOffset + (chr - 'a');
            } else {
                break;
            }
        } else if (base == BaseOct) {
            if (chr >= '0' && chr <= '7') {
                value = chr - '0';
            } else {
                break;
            }
        } else if (base == BaseBin) {
            if (chr == '0' || chr == '1') {
                value = chr - '0';
            } else {
                break;
            }
        }
        val = (val * static_cast<unsigned long long>(base)) + static_cast<unsigned long long>(value);
        advance();
        ++digits;
    }
    if (digits == 0) {
        std::ostringstream oss;
        oss << "Invalid & literal at " << line << ':' << col;
        throw LexError(oss.str());
    }
    const Token tok(TokenType::Integer, std::to_string(val), line, col);
    emitToken(out, tok);
    bol_ = false;
}

} // namespace gwbasic
