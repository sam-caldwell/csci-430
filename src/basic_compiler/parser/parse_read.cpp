// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/RestoreStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseRead() {
    // READ var[,var...] where var is identifier or A(expr)
    int l = peek().line, c = peek().col;
    std::vector<ReadTarget> targets;
    auto parseOne = [&]() {
        if (!check(TokenType::Identifier)) throw ParseError("Expected variable name in READ");
        ReadTarget t{};
        t.name = peek().lexeme; advance();
        if (match(TokenType::LParen)) {
            t.index = parseExpression();
            consume(TokenType::RParen, ")");
        }
        targets.push_back(std::move(t));
    };
    parseOne();
    while (match(TokenType::Comma)) parseOne();
    return make_node<ReadStmt>({l, c}, std::move(targets));
}

std::unique_ptr<Stmt> Parser::parseRestore() {
    return make_node<RestoreStmt>({peek().line, peek().col});
}

} // namespace gwbasic
