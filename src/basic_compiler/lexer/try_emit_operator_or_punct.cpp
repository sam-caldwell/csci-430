// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <array>
#include <string_view>
#include <vector>

namespace gwbasic {

/*
 * Function: Lexer::tryEmitOperatorOrPunct
 * Summary:
 *  Emit a recognized operator/punctuation token at the current position.
 * Parameters:
 *  - out: Token vector to append the emitted token to
 *  - line: Source line for the token's starting position
 *  - col: Source column for the token's starting position
 *  - c: Current character to classify
 * Returns:
 *  - bool: true if a token was emitted; false if unrecognized
 */
bool Lexer::tryEmitOperatorOrPunct(std::vector<Token>& out, int line, int col, const char chr) {
    switch (chr) {
        case Symbols::PLUS.first():       advance(); emitFixed<TokenType::Plus>(out, "+", line, col); break;
        case Symbols::MINUS.first():      advance(); emitFixed<TokenType::Minus>(out, "-", line, col); break;
        case Symbols::STAR.first():       advance(); emitFixed<TokenType::Star>(out, "*", line, col); break;
        case Symbols::SLASH.first():      advance(); emitFixed<TokenType::Slash>(out, "/", line, col); break;
        case '\\':                       advance(); emitFixed<TokenType::Backslash>(out, "\\", line, col); break;
        case Symbols::LPAREN.first():     advance(); emitFixed<TokenType::LParen>(out, "(", line, col); break;
        case Symbols::RPAREN.first():     advance(); emitFixed<TokenType::RParen>(out, ")", line, col); break;
        case Symbols::COLON.first():      advance(); emitFixed<TokenType::Colon>(out, ":", line, col); break;
        case Symbols::SEMICOLON.first():  advance(); emitFixed<TokenType::Semicolon>(out, ";", line, col); break;
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
                emitPairOrSingle<TokenType::Less, TokenType::LessEqual, Symbols::EQUALS.first()>(
                    out, std::array<std::string_view, 2>{"<", "<="}, line, col);
            }
            break;
        case Symbols::GREATER_THAN.first():
            advance();
            emitPairOrSingle<TokenType::Greater, TokenType::GreaterEqual, Symbols::EQUALS.first()>(
                out, std::array<std::string_view, 2>{">", ">="}, line, col);
            break;
        default:
            return false;
    }
    bol_ = false;
    return true;
}

} // namespace gwbasic
