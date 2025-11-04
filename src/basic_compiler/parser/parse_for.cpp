// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ForStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseFor
 * Inputs:
 *  - none (assumes FOR already consumed)
 * Outputs:
 *  - ForStmt: loop construct with optional STEP and body
 * Theory of operation:
 *  - Parses induction variable, start expression, TO end expression,
 *    optional STEP, then collects statements until NEXT on the same line.
 *    If end-of-line is reached before NEXT, treat as a multi-line FOR;
 *    the parser will fold subsequent lines into the body during
 *    parseProgram restructuring.
 */
std::unique_ptr<Stmt> Parser::parseFor() {
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
    auto node = make_node<ForStmt>({l, c}, var, std::move(start), std::move(end), std::move(step));
    while (!check(TokenType::KwNext)) {
        if (check(TokenType::NewLine) || atEnd()) {
            // Multi-line FOR: stop collecting inline body; NEXT will appear on a later line
            return node;
        }
        if (match(TokenType::Colon)) continue;
        node->body.push_back(parseStatement());
        if (match(TokenType::Colon)) continue;
    }
    consume(TokenType::KwNext, "NEXT");
    if (check(TokenType::Identifier)) {
        advance();
    }
    node->inlineNext = true;
    return node;
}

} // namespace gwbasic
