// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseComparison
 * Summary:
 *  Parse comparison expressions: =, <>, <, <=, >, >=.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Expr>: Expression node with comparison precedence
 */
std::unique_ptr<Expr> Parser::parseComparison() {
    auto left = parseTerm();
    while (true) {
        if (match(TokenType::Assign)) {
            auto right = parseTerm();
            left = std::make_unique<BinaryExpr>(BinaryOp::Eq, std::move(left), std::move(right));
        } else if (match(TokenType::NotEqual)) {
            auto right = parseTerm();
            left = std::make_unique<BinaryExpr>(BinaryOp::Ne, std::move(left), std::move(right));
        } else if (match(TokenType::LessEqual)) {
            auto right = parseTerm();
            left = std::make_unique<BinaryExpr>(BinaryOp::Le, std::move(left), std::move(right));
        } else if (match(TokenType::GreaterEqual)) {
            auto right = parseTerm();
            left = std::make_unique<BinaryExpr>(BinaryOp::Ge, std::move(left), std::move(right));
        } else if (match(TokenType::Less)) {
            auto right = parseTerm();
            left = std::make_unique<BinaryExpr>(BinaryOp::Lt, std::move(left), std::move(right));
        } else if (match(TokenType::Greater)) {
            auto right = parseTerm();
            left = std::make_unique<BinaryExpr>(BinaryOp::Gt, std::move(left), std::move(right));
        } else {
            break;
        }
    }
    return left;
}

} // namespace gwbasic
