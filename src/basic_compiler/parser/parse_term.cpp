// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseTerm
 * Summary:
 *  Parse additive-precedence expressions: + and -.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Expr>: Expression tree for additive precedence
 */
std::unique_ptr<Expr> Parser::parseTerm() {
    auto left = parseFactor();
    while (true) {
        if (match(TokenType::Plus)) {
            auto right = parseFactor();
            left = std::make_unique<BinaryExpr>(BinaryOp::Add, std::move(left), std::move(right));
        } else if (match(TokenType::Minus)) {
            auto right = parseFactor();
            left = std::make_unique<BinaryExpr>(BinaryOp::Sub, std::move(left), std::move(right));
        } else {
            break;
        }
    }
    return left;
}

} // namespace gwbasic
