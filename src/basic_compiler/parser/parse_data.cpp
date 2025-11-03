// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StringExpr.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseData() {
    // DATA item[,item...]; items must be string or numeric literals
    int l = peek().line, c = peek().col;
    std::vector<std::string> items;
    bool first = true;
    while (true) {
        // Stop at end of statement
        if (!first && !(check(TokenType::Comma))) break;
        if (!first) advance();
        first = false;
        // Accept string or number
        if (check(TokenType::String)) {
            items.push_back(peek().lexeme);
            advance();
        } else if (check(TokenType::Integer) || check(TokenType::Float)) {
            items.push_back(peek().lexeme);
            advance();
        } else {
            throw ParseError("DATA expects literal items");
        }
        // If next token is end-of-statement, stop
        if (check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile)) break;
    }
    return make_node<DataStmt>({l, c}, std::move(items));
}

} // namespace gwbasic

