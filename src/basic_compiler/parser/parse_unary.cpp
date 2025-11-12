// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: Parser::parseUnary
 * Summary:
 *  Parse unary +, -, and NOT applied to the next expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Expr>: Unary expression or next precedence level
 */
std::unique_ptr<Expr> Parser::parseUnary() {
    if (match(TokenType::Plus)) return std::make_unique<UnaryExpr>(Symbols::PLUS.first(), parseUnary());
    if (match(TokenType::Minus)) return std::make_unique<UnaryExpr>(Symbols::MINUS.first(), parseUnary());
    if (match(TokenType::KwNot)) return std::make_unique<UnaryExpr>(Symbols::EXCLAMATION.first(), parseUnary());
    return parsePower();
}

} // namespace gwbasic
