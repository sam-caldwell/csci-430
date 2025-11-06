// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"

namespace gwbasic {

/*
 * Function: Parser::parsePower
 * Inputs:
 *  - none
 * Outputs:
 *  - Expr: expression tree for exponentiation (right-associative)
 * Theory of operation:
 *  - Parses a primary expression, then if '^' follows, parses the RHS via
 *    parseUnary() to allow unary signs in exponents, and constructs a
 *    BinaryExpr(Pow) combining them. Right associativity is achieved by
 *    delegating RHS back through parseUnary() which recurses into parsePower().
 */
std::unique_ptr<Expr> Parser::parsePower() {
    auto left = parsePrimary();
    if (match(TokenType::Caret)) {
        auto right = parseUnary();
        return std::make_unique<BinaryExpr>(BinaryOp::Pow, std::move(left), std::move(right));
    }
    return left;
}

} // namespace gwbasic

