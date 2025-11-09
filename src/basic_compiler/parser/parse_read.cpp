// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ReadStmt.h"

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
std::unique_ptr<Stmt> Parser::parseRead() {
    const int l = peek().line;
    // READ var[,var...] where var is identifier or A(expr)
    const int c = peek().col;
    std::vector<ReadTarget> targets;
    auto parseOne = [&]() {
        if (!check(TokenType::Identifier)) throw ParseError("Expected variable name in READ");
        ReadTarget t{};
        t.name = peek().lexeme; advance();
        if (match(TokenType::LParen)) {
            if (!check(TokenType::RParen)) {
                do {
                    t.indices.push_back(parseExpression());
                } while (match(TokenType::Comma));
            }
            consume(TokenType::RParen, ")");
        }
        targets.push_back(std::move(t));
    };
    parseOne();
    while (match(TokenType::Comma)) parseOne();
    return make_node<ReadStmt>({l, c}, std::move(targets));
}

} // namespace gwbasic
