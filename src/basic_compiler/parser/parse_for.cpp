// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseFor
 * Summary:
 *  Parse FOR var = start TO end [STEP step] with inline or multiline body.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ForStmt with optional STEP and body
 */
std::unique_ptr<Stmt> Parser::parseFor() {
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected variable name after FOR");
    }
    const std::string var = peek().lexeme;
    const int lineNum = peek().line;
    const int colNum = peek().col;
    advance();
    consume(TokenType::Assign, "'='");
    auto start = parseExpression();
    consume(TokenType::KwTo, "TO");
    auto end = parseExpression();
    std::unique_ptr<Expr> step;
    if (match(TokenType::KwStep)) {
        step = parseExpression();
    }
    auto node = make_node<ForStmt>({lineNum, colNum}, var, std::move(start), std::move(end), std::move(step));
    while (!check(TokenType::KwNext)) {
        if (check(TokenType::NewLine) || atEnd()) {
            // Multi-line FOR: stop collecting inline body; NEXT will appear on a later line
            return node;
        }
        if (match(TokenType::Colon)) { continue; }
        node->body.push_back(parseStatement());
        if (match(TokenType::Colon)) { continue; }
    }
    consume(TokenType::KwNext, "NEXT");
    if (check(TokenType::Identifier)) {
        advance();
    }
    node->inlineNext = true;
    return node;
}

} // namespace gwbasic
