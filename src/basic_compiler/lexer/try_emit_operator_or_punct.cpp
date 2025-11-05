// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"

namespace gwbasic {

/*
 * Function: Lexer::tryEmitOperatorOrPunct
 * Purpose:
 *  - Consume and emit a recognized single- or two-character operator or
 *    punctuation token. Leaves unexpected characters to the caller.
 * Outputs:
 *  - bool: true if a token was emitted; false if character unrecognized
 */
bool Lexer::tryEmitOperatorOrPunct(std::vector<Token>& out, int line, int col, const char c) {
    switch (c) {
        case SYM_PLUS:       advance(); emitFixed<TokenType::Plus>(out, "+", line, col); break;
        case SYM_MINUS:      advance(); emitFixed<TokenType::Minus>(out, "-", line, col); break;
        case SYM_STAR:       advance(); emitFixed<TokenType::Star>(out, "*", line, col); break;
        case SYM_SLASH:      advance(); emitFixed<TokenType::Slash>(out, "/", line, col); break;
        case SYM_LPAREN:     advance(); emitFixed<TokenType::LParen>(out, "(", line, col); break;
        case SYM_RPAREN:     advance(); emitFixed<TokenType::RParen>(out, ")", line, col); break;
        case SYM_COLON:      advance(); emitFixed<TokenType::Colon>(out, ":", line, col); break;
        case SYM_COMMA:      advance(); emitFixed<TokenType::Comma>(out, ",", line, col); break;
        case SYM_HASH:       advance(); emitFixed<TokenType::Hash>(out, "#", line, col); break;
        case SYM_EQUALS:     advance(); emitFixed<TokenType::Assign>(out, "=", line, col); break;
        case SYM_LESS_THAN:
            advance();
            if (peek() == SYM_GREATER_THAN) {
                advance(); emitFixed<TokenType::NotEqual>(out, "<>", line, col);
            }
            else {
                emitPairOrSingle<TokenType::Less, TokenType::LessEqual, SYM_EQUALS>(out, "<", "<=", line, col);
            }
            break;
        case SYM_GREATER_THAN:
            advance();
            emitPairOrSingle<TokenType::Greater, TokenType::GreaterEqual, SYM_EQUALS>(out, ">", ">=", line, col);
            break;
        default:
            return false;
    }
    bol_ = false;
    return true;
}

} // namespace gwbasic
