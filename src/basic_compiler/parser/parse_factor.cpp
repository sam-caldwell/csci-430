// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

std::unique_ptr<Expr> Parser::parseFactor() {
    /*
     * Function: Parser::parseFactor
     * Inputs:
     *  - none
     * Outputs:
     *  - Expr: expression tree for multiplicative precedence
     * Theory of operation:
     *  - Parses a unary expression, then folds '*' and '/' operators
     *    left-associatively.
     */
    auto left = parseUnary();
    while (true) {
        if (match(TokenType::Star)) { auto right = parseUnary(); left = std::make_unique<BinaryExpr>(BinaryOp::Mul, std::move(left), std::move(right)); }
        else if (match(TokenType::Slash)) { auto right = parseUnary(); left = std::make_unique<BinaryExpr>(BinaryOp::Div, std::move(left), std::move(right)); }
        else break;
    }
    return left;
}

} // namespace gwbasic
