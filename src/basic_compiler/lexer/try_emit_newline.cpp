// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <vector>

namespace gwbasic {

/*
 * Function: Lexer::tryEmitNewline
 * Purpose:
 *  - If the current character is a newline, consume it and emit a
 *    NewLine token, updating beginning-of-line state.
 * Outputs:
 *  - bool: true if a newline was consumed and emitted; false otherwise
 */
bool Lexer::tryEmitNewline(std::vector<Token>& out) {
    if (peek() != Symbols::LF.first()) {
        return false;
    }
    advance();
    emitFixed<TokenType::NewLine>(out, Symbols::LF, line_ - 1, 1);
    bol_ = true;
    return true;
}

} // namespace gwbasic
