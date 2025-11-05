// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseAssignOrLet
 * Purpose:
 *  - Parse assignment statements with optional leading LET keyword.
 * Inputs:
 *  - none (assumes current token is LET or Identifier)
 * Outputs:
 *  - AssignStmt or ArrayAssignStmt depending on presence of '(index)'
 */
std::unique_ptr<Stmt> Parser::parseAssignOrLet() {
    if (match(TokenType::KwLet)) {
        // proceed to identifier
    }
    if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after LET");
    std::string name = peek().lexeme;
    const int l = peek().line;
    int c = peek().col;
    advance();
    // Array element assignment A(expr) = ...
    if (match(TokenType::LParen)) {
        auto idx = parseExpression();
        consume(TokenType::RParen, ")");
        consume(TokenType::Assign, "'='");
        auto expr = parseExpression();
        return make_node<ArrayAssignStmt>({l, c}, name, std::move(idx), std::move(expr));
    }
    consume(TokenType::Assign, "'='");
    auto expr = parseExpression();
    return make_node<AssignStmt>({l, c}, name, std::move(expr));
}

} // namespace gwbasic
