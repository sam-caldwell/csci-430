// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include <cctype>

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
    const unsigned char cu = static_cast<unsigned char>(peek());
    if (std::isdigit(cu) || (peek() == '.' && std::isdigit(static_cast<unsigned char>(peekNext())))) {
        const Token t = number();
        emitToken(out, t);
        bol_ = false;
        return true;
    }
    if (std::isalpha(cu)) {
        const Token t = identifierOrKeyword();
        emitToken(out, t);
        bol_ = false;
        return true;
    }
    if (peek() == '"') {
        const Token t = stringLiteral();
        emitToken(out, t);
        bol_ = false;
        return true;
    }
    return false;
}

} // namespace gwbasic
