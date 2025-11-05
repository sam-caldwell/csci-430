// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include <cctype>

namespace gwbasic {

/*
 * Function: Parser::tryParseSpecialIdentifierStatement
 * Inputs:
 *  - startTok: Token at which the statement begins (for pos assignment)
 * Outputs:
 *  - std::unique_ptr<Stmt> when a special identifier-led statement is parsed;
 *    otherwise nullptr and no tokens are consumed.
 * Theory of operation:
 *  - Some statements are introduced by identifiers rather than keywords
 *    (e.g., SCREEN, CIRCLE). When these appear and are not function calls,
 *    parse them before falling back to generic identifier assignment parsing.
 */
std::unique_ptr<Stmt> Parser::tryParseSpecialIdentifierStatement(const Token& startTok) {
    if (!check(TokenType::Identifier)) return nullptr;
    std::string up = peek().lexeme;
    for (auto &ch: up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    if (up == "SCREEN") {
        if (peekNext().type != TokenType::LParen) {
            advance();
            auto n = parseScreen();
            n->pos = {startTok.line, startTok.col};
            return n;
        }
        return nullptr; // function form: not a statement
    }
    if (up == "CIRCLE") {
        advance();
        auto n = parseCircle();
        n->pos = {startTok.line, startTok.col};
        return n;
    }
    return nullptr;
}

} // namespace gwbasic

