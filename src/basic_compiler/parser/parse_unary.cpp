// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: Parser::parseUnary
 * Inputs:
 *  - none
 * Outputs:
 *  - Expr: unary +/- applied to a primary expression
 * Theory of operation:
 *  - Recognizes leading '+' or '-' and constructs a UnaryExpr; otherwise
 *    defers to parsePrimary().
 */
std::unique_ptr<Expr> Parser::parseUnary() {
    if (match(TokenType::Plus)) return std::make_unique<UnaryExpr>(Symbols::PLUS.first(), parseUnary());
    if (match(TokenType::Minus)) return std::make_unique<UnaryExpr>(Symbols::MINUS.first(), parseUnary());
    if (match(TokenType::KwNot)) return std::make_unique<UnaryExpr>(Symbols::EXCLAMATION.first(), parseUnary());
    return parsePower();
}

} // namespace gwbasic
