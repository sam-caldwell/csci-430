// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseLogical
 * Summary:
 *  Parse left-associative logical AND/OR over comparison expressions.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Expr>: Expression node
 */
std::unique_ptr<Expr> Parser::parseLogical() {
    auto left = parseComparison();
    while (true) {
        if (match(TokenType::KwAnd)) {
            auto right = parseComparison();
            left = std::make_unique<BinaryExpr>(BinaryOp::And, std::move(left), std::move(right));
        } else if (match(TokenType::KwOr)) {
            auto right = parseComparison();
            left = std::make_unique<BinaryExpr>(BinaryOp::Or, std::move(left), std::move(right));
        } else {
            break;
        }
    }
    return left;
}

} // namespace gwbasic
