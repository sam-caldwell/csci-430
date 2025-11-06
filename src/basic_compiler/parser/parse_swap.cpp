// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/SwapStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseSwap
 * Purpose:
 *  - Parse SWAP varref, varref where varref may be a scalar or array element.
 * Inputs:
 *  - none (assumes 'SWAP' matched by caller)
 * Outputs:
 *  - SwapStmt with left and right ReadTarget references
 */
std::unique_ptr<Stmt> Parser::parseSwap() {
    const int l = peek().line;
    const int c = peek().col;
    auto parseVarRef = [&]() -> ReadTarget {
        if (!check(TokenType::Identifier)) throw ParseError("Expected variable name in SWAP");
        ReadTarget t{};
        t.name = peek().lexeme; advance();
        if (match(TokenType::LParen)) {
            if (!check(TokenType::RParen)) {
                do { t.indices.push_back(parseExpression()); } while (match(TokenType::Comma));
            }
            consume(TokenType::RParen, ")");
        }
        return t;
    };
    ReadTarget left = parseVarRef();
    consume(TokenType::Comma, ",");
    ReadTarget right = parseVarRef();
    return make_node<SwapStmt>({l, c}, std::move(left), std::move(right));
}

} // namespace gwbasic

