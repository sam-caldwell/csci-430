// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ReadTarget.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/SwapStmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

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
std::unique_ptr<Stmt> Parser::parseSwap() { // NOLINT(readability-function-size)
    const int line = peek().line;
    const int col = peek().col;
    auto parseVarRef = [&]() -> ReadTarget {
        if (!check(TokenType::Identifier)) {
            throw ParseError("Expected variable name in SWAP");
        }
        ReadTarget target{};
        target.name = peek().lexeme;
        advance();
        if (match(TokenType::LParen)) {
            if (!check(TokenType::RParen)) {
                target.indices.push_back(parseExpression());
                while (match(TokenType::Comma)) { target.indices.push_back(parseExpression()); }
            }
            consume(TokenType::RParen, ")");
        }
        return target;
    };
    ReadTarget left = parseVarRef();
    consume(TokenType::Comma, ",");
    ReadTarget right = parseVarRef();
    return make_node<SwapStmt>({line, col}, std::move(left), std::move(right));
}

} // namespace gwbasic
