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
    // Syntax (GW-BASIC compatible):
    // CIRCLE [STEP] (x,y), r [, color] [, start] [, end] [, aspect]
    bool step = false;
    if (match(TokenType::KwStep)) step = true;
    consume(TokenType::LParen, "'('");
    auto x = parseExpression();
    consume(TokenType::Comma, ",");
    auto y = parseExpression();
    consume(TokenType::RParen, "')'");
    consume(TokenType::Comma, ",");
    auto r = parseExpression();

    std::unique_ptr<Expr> color, start, end, aspect;

    auto canStartExpr = [&]() {
        return !(
            check(TokenType::NewLine) ||
            check(TokenType::Colon) ||
            check(TokenType::EndOfFile) ||
            check(TokenType::Comma)
        );
    };
    if (match(TokenType::Comma)) {
        // color or empty
        if (canStartExpr()) { color = parseExpression(); }
        if (match(TokenType::Comma)) {
            // start or empty
            if (canStartExpr()) { start = parseExpression(); }
            if (match(TokenType::Comma)) {
                // end or empty
                if (canStartExpr()) { end = parseExpression(); }
                if (match(TokenType::Comma)) {
                    if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
                        aspect = parseExpression();
                    }
                }
            }
        }
    }
    return make_node<CircleStmt>({0,0}, std::move(x), std::move(y), std::move(r),
                                 std::move(color), std::move(start), std::move(end), std::move(aspect), step);
}

} // namespace gwbasic
