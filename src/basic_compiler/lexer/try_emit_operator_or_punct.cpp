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
        case Symbols::PLUS.first():       advance(); emitFixed<TokenType::Plus>(out, "+", line, col); break;
        case Symbols::MINUS.first():      advance(); emitFixed<TokenType::Minus>(out, "-", line, col); break;
        case Symbols::STAR.first():       advance(); emitFixed<TokenType::Star>(out, "*", line, col); break;
        case Symbols::SLASH.first():      advance(); emitFixed<TokenType::Slash>(out, "/", line, col); break;
        case '\\':                       advance(); emitFixed<TokenType::Backslash>(out, "\\", line, col); break;
        case Symbols::LPAREN.first():     advance(); emitFixed<TokenType::LParen>(out, "(", line, col); break;
        case Symbols::RPAREN.first():     advance(); emitFixed<TokenType::RParen>(out, ")", line, col); break;
        case Symbols::COLON.first():      advance(); emitFixed<TokenType::Colon>(out, ":", line, col); break;
        case Symbols::COMMA.first():      advance(); emitFixed<TokenType::Comma>(out, ",", line, col); break;
        case Symbols::HASH.first():       advance(); emitFixed<TokenType::Hash>(out, "#", line, col); break;
        case Symbols::CARET.first():      advance(); emitFixed<TokenType::Caret>(out, "^", line, col); break;
        case Symbols::EQUALS.first():     advance(); emitFixed<TokenType::Assign>(out, "=", line, col); break;
        case Symbols::LESS_THAN.first():
            advance();
            if (peek() == Symbols::GREATER_THAN.first()) {
                advance(); emitFixed<TokenType::NotEqual>(out, "<>", line, col);
            }
            else {
                emitPairOrSingle<TokenType::Less, TokenType::LessEqual, Symbols::EQUALS.first()>(out, "<", "<=", line, col);
            }
            break;
        case Symbols::GREATER_THAN.first():
            advance();
            emitPairOrSingle<TokenType::Greater, TokenType::GreaterEqual, Symbols::EQUALS.first()>(out, ">", ">=", line, col);
            break;
        default:
            return false;
    }
    bol_ = false;
    return true;
}

} // namespace gwbasic
