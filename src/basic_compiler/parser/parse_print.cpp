// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

namespace gwbasic {

/*
 * Function: Parser::parsePrint
 * Inputs:
 *  - none (assumes PRINT already consumed)
 * Outputs:
 *  - PrintStmt: printing either a string literal or an expression
 * Theory of operation:
 *  - If the next token is STRING, returns a PrintStmt with StringExpr;
 *    otherwise parses an expression and returns a PrintStmt for numeric
 *    output.
 */
std::unique_ptr<Stmt> Parser::parsePrint() {
    const int l = peek().line;
    const int c = peek().col;
    int channel = -1;
    // Optional: PRINT # n ,
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
        channel = std::stoi(peek().lexeme); advance();
        if (match(TokenType::Comma)) {}
    }
    // Optional: USING formatExpr ,
    std::unique_ptr<Expr> fmt;
    if (match(TokenType::KwUsing)) {
        fmt = parseExpression();
        if (match(TokenType::Comma)) {}
    }
    // Parse one or more expressions separated by commas
    std::vector<std::unique_ptr<Expr>> items;
    items.push_back(parseExpression());
    while (match(TokenType::Comma)) items.push_back(parseExpression());
    auto node = make_node<PrintStmt>({l, c}, std::move(items));
    node->channel = channel;
    node->format = std::move(fmt);
    return node;
}

} // namespace gwbasic
