// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ListStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>

namespace gwbasic {

/*
 * Function: Parser::parseList
 * Summary:
 *  Parse LIST statement with optional start/end line specifiers.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ListStmt with range and toPrinter=false
 */
std::unique_ptr<Stmt> Parser::parseList() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    auto node = make_node<ListStmt>({peek().line, peek().col});
    // Optional: start
    auto parseLineRefHere = [this](int& outVal, bool& isDot) -> bool {
        if (check(TokenType::Integer)) {
            outVal = std::stoi(peek().lexeme);
            advance();
            isDot = false;
            return true;
        }
        if (check(TokenType::Identifier) && peek().lexeme == ".") {
            advance();
            outVal = -1;
            isDot = true;
            return true;
        }
        return false;
    };
    int startVal = 0; bool startIsDot = false;
    if (parseLineRefHere(startVal, startIsDot)) {
        node->startLine = startVal; node->startIsDot = startIsDot;
        // Optional range indicator '-'
        if (match(TokenType::Minus)) {
            // Optional end
            int endVal = 0; bool endIsDot = false;
            if (parseLineRefHere(endVal, endIsDot)) {
                node->endLine = endVal; node->endIsDot = endIsDot;
            } else {
                // Open-ended range: no endLine
            }
        } else {
            // Single-line listing when only one ref is given
            node->endLine = startVal; node->endIsDot = startIsDot;
        }
    }
    node->toPrinter = false;
    return node;
}

/*
 * Function: Parser::parseLlist
 * Summary:
 *  Parse LLIST statement (PRINT listing to printer) with optional ranges.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ListStmt with range and toPrinter=true
 */
std::unique_ptr<Stmt> Parser::parseLlist() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    auto node = make_node<ListStmt>({peek().line, peek().col});
    auto parseLineRefHere = [this](int& outVal, bool& isDot) -> bool {
        if (check(TokenType::Integer)) {
            outVal = std::stoi(peek().lexeme);
            advance();
            isDot = false;
            return true;
        }
        if (check(TokenType::Identifier) && peek().lexeme == ".") {
            advance();
            outVal = -1;
            isDot = true;
            return true;
        }
        return false;
    };
    int startVal = 0; bool startIsDot = false;
    if (parseLineRefHere(startVal, startIsDot)) {
        node->startLine = startVal; node->startIsDot = startIsDot;
        if (match(TokenType::Minus)) {
            int endVal = 0; bool endIsDot = false;
            if (parseLineRefHere(endVal, endIsDot)) {
                node->endLine = endVal; node->endIsDot = endIsDot;
            }
        } else {
            node->endLine = startVal; node->endIsDot = startIsDot;
        }
    }
    node->toPrinter = true;
    return node;
}

} // namespace gwbasic
