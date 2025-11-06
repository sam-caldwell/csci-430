// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/WriteStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseWrite
 * Purpose:
 *  - Parse WRITE [#n,] expr[,expr...]
 * Inputs:
 *  - none (assumes 'WRITE' matched by caller)
 * Outputs:
 *  - WriteStmt: optional channel number and list of expressions to output
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
