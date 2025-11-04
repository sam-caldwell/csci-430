// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/CircleStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseCircle
 * Purpose:
 *  - Parse CIRCLE x, y, r (minimal stub grammar)
 * Outputs:
 *  - CircleStmt with numeric expressions for x, y and r
 */
std::unique_ptr<Stmt> Parser::parseCircle() {
    auto x = parseExpression();
    consume(TokenType::Comma, ",");
    auto y = parseExpression();
    consume(TokenType::Comma, ",");
    auto r = parseExpression();
    return make_node<CircleStmt>({0,0}, std::move(x), std::move(y), std::move(r));
}

} // namespace gwbasic

