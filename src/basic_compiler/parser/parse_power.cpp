// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"

namespace gwbasic {

/*
 * Function: Parser::parsePower
 * Summary:
 *  Parse a right-associative exponentiation expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Expr>: Expression tree for exponentiation
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
