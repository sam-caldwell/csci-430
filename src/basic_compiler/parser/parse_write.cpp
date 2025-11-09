// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/WriteStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseWrite
 * Summary:
 *  Parse WRITE [#n,] expr[,expr...] into a WriteStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: WriteStmt with channel and expressions
 */
std::unique_ptr<Stmt> Parser::parseWrite() {
    const int l = peek().line;
    const int c = peek().col;
    int channel = -1;
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
        channel = std::stoi(peek().lexeme); advance();
        if (match(TokenType::Comma)) {}
    }
    std::vector<std::unique_ptr<Expr>> items;
    items.push_back(parseExpression());
    while (match(TokenType::Comma)) items.push_back(parseExpression());
    return make_node<WriteStmt>({l, c}, channel, std::move(items));
}

} // namespace gwbasic
