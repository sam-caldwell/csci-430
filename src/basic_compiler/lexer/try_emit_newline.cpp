// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"

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
    if (peek() != CH_LF) return false;
    advance();
    emitFixed<TokenType::NewLine>(out, STR_LF, line_ - 1, 1);
    bol_ = true;
    return true;
}

} // namespace gwbasic

