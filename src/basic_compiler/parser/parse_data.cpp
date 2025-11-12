// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DataItem.h"
#include "basic_compiler/ast/DataStmt.h"
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
 * Function: Parser::parseData
 * Summary:
 *  Parse DATA item[,item...] where items are literal numbers or strings.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: DataStmt containing the literal items
 */
std::unique_ptr<Stmt> Parser::parseData() {
    // DATA item[,item...]; items must be string or numeric literals
    const int line = peek().line;
    const int col = peek().col;
    std::vector<DataItem> items;
    bool first = true;
    while (true) {
        // Stop at end of statement
        if (!first && !(check(TokenType::Comma))) {
            break;
        }
        if (!first) {
            advance();
        }
        first = false;
        // Accept string or number
        if (check(TokenType::String)) {
            DataItem item; item.isString = true; item.text = peek().lexeme; items.push_back(std::move(item));
            advance();
        } else if (check(TokenType::Integer) || check(TokenType::Float)) {
            DataItem item; item.isString = false; item.text = peek().lexeme; items.push_back(std::move(item));
            advance();
        } else {
            throw ParseError("DATA expects literal items");
        }
        // If next token is end-of-statement, stop
        if (check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile)) {
            break;
        }
    }
    return make_node<DataStmt>({line, col}, std::move(items));
}

} // namespace gwbasic
