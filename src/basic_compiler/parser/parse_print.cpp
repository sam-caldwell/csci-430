// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parsePrint() {
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
    // Parse one or more expressions separated by commas
    std::vector<std::unique_ptr<Expr>> items;
    int l = peek().line, c = peek().col;
    // First expression (string literal or general expression)
    items.push_back(parseExpression());
    // Additional items separated by commas
    while (match(TokenType::Comma)) {
        items.push_back(parseExpression());
    }
    return make_node<PrintStmt>({l, c}, std::move(items));
}

} // namespace gwbasic
