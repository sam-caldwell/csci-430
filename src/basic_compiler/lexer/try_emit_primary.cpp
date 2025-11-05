// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <cctype>

namespace gwbasic {

/*
 * Function: Lexer::tryEmitPrimary
 * Purpose:
 *  - Try to emit a primary token category starting at the current
 *    position: number, identifier/keyword, or string literal.
 * Outputs:
 *  - bool: true if a token was emitted; false if none matched
 */
bool Lexer::tryEmitPrimary(std::vector<Token>& out) {
    const unsigned char cu = static_cast<unsigned char>(peek());
    if (std::isdigit(cu)) {
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

