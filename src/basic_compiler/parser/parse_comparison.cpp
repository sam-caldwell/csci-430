// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

std::unique_ptr<Expr> Parser::parseComparison() {
    /*
     * Function: Parser::parseComparison
     * Inputs:
     *  - none
     * Outputs:
     *  - Expr: expression tree with comparison operators at this precedence
     * Theory of operation:
     *  - Parses a term, then folds zero or more relational operators (=, <>,
     *    <, <=, >, >=) with right operands as BinaryExpr nodes.
     */
    auto left = parseTerm();
    while (true) {
        if (match(TokenType::Assign)) { auto right = parseTerm(); left = std::make_unique<BinaryExpr>(BinaryOp::Eq, std::move(left), std::move(right)); }
        else if (match(TokenType::NotEqual)) { auto right = parseTerm(); left = std::make_unique<BinaryExpr>(BinaryOp::Ne, std::move(left), std::move(right)); }
        else if (match(TokenType::LessEqual)) { auto right = parseTerm(); left = std::make_unique<BinaryExpr>(BinaryOp::Le, std::move(left), std::move(right)); }
        else if (match(TokenType::GreaterEqual)) { auto right = parseTerm(); left = std::make_unique<BinaryExpr>(BinaryOp::Ge, std::move(left), std::move(right)); }
        else if (match(TokenType::Less)) { auto right = parseTerm(); left = std::make_unique<BinaryExpr>(BinaryOp::Lt, std::move(left), std::move(right)); }
        else if (match(TokenType::Greater)) { auto right = parseTerm(); left = std::make_unique<BinaryExpr>(BinaryOp::Gt, std::move(left), std::move(right)); }
        else break;
    }
    return left;
}

} // namespace gwbasic
