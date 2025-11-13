// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/token/Token.h"
#include <cctype>
#include <vector>

namespace gwbasic {

/*
 * Function: Lexer::tryEmitPrimary
 * Summary:
 *  Try to emit a number, identifier/keyword, or string literal token.
 * Parameters:
 *  - out: Token vector to append the emitted token to
 * Returns:
 *  - bool: true if a token was emitted; false otherwise
 */
bool Lexer::tryEmitPrimary(std::vector<Token>& out) {
    const unsigned char cur = static_cast<unsigned char>(peek());
    if ((std::isdigit(cur) != 0) || (peek() == '.' && (std::isdigit(static_cast<unsigned char>(peekNext())) != 0))) {
        const Token tok = number();
        emitToken(out, tok);
        bol_ = false;
        return true;
    }
    if (std::isalpha(cur) != 0) {
        const Token tok = identifierOrKeyword();
        emitToken(out, tok);
        bol_ = false;
        return true;
    }
    if (peek() == '"') {
        const Token tok = stringLiteral();
        emitToken(out, tok);
        bol_ = false;
        return true;
    }
    return false;
}

} // namespace gwbasic
