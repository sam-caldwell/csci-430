// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

std::unique_ptr<Expr> Parser::parseTerm() {
    /*
     * Function: Parser::parseTerm
     * Inputs:
     *  - none
     * Outputs:
     *  - Expr: expression tree for additive precedence
     * Theory of operation:
     *  - Parses a factor, then folds '+' and '-' operators left-associatively.
     */
    auto left = parseFactor();
    while (true) {
        if (match(TokenType::Plus)) { auto right = parseFactor(); left = std::make_unique<BinaryExpr>(BinaryOp::Add, std::move(left), std::move(right)); }
        else if (match(TokenType::Minus)) { auto right = parseFactor(); left = std::make_unique<BinaryExpr>(BinaryOp::Sub, std::move(left), std::move(right)); }
        else break;
    }
    return left;
}

} // namespace gwbasic
