// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

std::unique_ptr<Expr> Parser::parseUnary() {
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
    if (match(TokenType::Plus)) return std::make_unique<UnaryExpr>('+', parseUnary());
    if (match(TokenType::Minus)) return std::make_unique<UnaryExpr>('-', parseUnary());
    return parsePrimary();
}

} // namespace gwbasic
