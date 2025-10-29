// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseFor() {
    /*
     * Function: Parser::parseFor
     * Inputs:
     *  - none (assumes FOR already consumed)
     * Outputs:
     *  - ForStmt: single-line loop construct with optional STEP and body
     * Theory of operation:
     *  - Parses induction variable, start expression, TO end expression,
     *    optional STEP, then collects statements until NEXT on the same line.
     */
    if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after FOR");
    std::string var = peek().lexeme;
    int l = peek().line, c = peek().col;
    advance();
    consume(TokenType::Assign, "'='");
    auto start = parseExpression();
    consume(TokenType::KwTo, "TO");
    auto end = parseExpression();
    std::unique_ptr<Expr> step;
    if (match(TokenType::KwStep)) {
        step = parseExpression();
    }
    auto node = std::make_unique<ForStmt>(var, std::move(start), std::move(end), std::move(step));
    while (!check(TokenType::KwNext)) {
        if (check(TokenType::NewLine) || atEnd()) {
            throw ParseError("FOR body must end with NEXT on the same line for now");
        }
        if (match(TokenType::Colon)) continue;
        node->body.push_back(parseStatement());
        if (match(TokenType::Colon)) continue;
    }
    consume(TokenType::KwNext, "NEXT");
    if (check(TokenType::Identifier)) {
        advance();
    }
    node->pos = {l, c};
    return node;
}

} // namespace gwbasic
