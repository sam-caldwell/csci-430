// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/ReadTarget.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parseRead
 * Summary:
 *  Parse READ var[,var...] where each target may be scalar or array element.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ReadStmt with target names and optional indices
 */
std::unique_ptr<Stmt> Parser::parseRead() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    const int lineNum = peek().line;
    // READ var[,var...] where var is identifier or A(expr)
    const int colNum = peek().col;
    std::vector<ReadTarget> targets;
    auto parseOne = [&]() {
        if (!check(TokenType::Identifier)) {
            throw ParseError("Expected variable name in READ");
        }
        ReadTarget target{};
        target.name = peek().lexeme;
        advance();
        if (match(TokenType::LParen)) {
            if (!check(TokenType::RParen)) {
                target.indices.push_back(parseExpression());
                while (match(TokenType::Comma)) {
                    target.indices.push_back(parseExpression());
                }
            }
            consume(TokenType::RParen, ")");
        }
        targets.push_back(std::move(target));
    };
    parseOne();
    while (match(TokenType::Comma)) {
        parseOne();
    }
    return make_node<ReadStmt>({lineNum, colNum}, std::move(targets));
}

} // namespace gwbasic
