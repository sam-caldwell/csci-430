// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ListStmt.h"

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
std::unique_ptr<Stmt> Parser::parseList() {
    auto node = make_node<ListStmt>({peek().line, peek().col});
    // Optional: start
    auto parseLineRefHere = [this](int& outVal, bool& isDot)->bool{
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
    int v = 0; bool dot = false;
    if (parseLineRefHere(v, dot)) {
        node->startLine = v; node->startIsDot = dot;
        // Optional range indicator '-'
        if (match(TokenType::Minus)) {
            // Optional end
            int v2 = 0; bool dot2 = false;
            if (parseLineRefHere(v2, dot2)) {
                node->endLine = v2; node->endIsDot = dot2;
            } else {
                // Open-ended range: no endLine
            }
        } else {
            // Single-line listing when only one ref is given
            node->endLine = v; node->endIsDot = dot;
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
std::unique_ptr<Stmt> Parser::parseLlist() {
    auto node = make_node<ListStmt>({peek().line, peek().col});
    auto parseLineRefHere = [this](int& outVal, bool& isDot)->bool{
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
    int v = 0; bool dot = false;
    if (parseLineRefHere(v, dot)) {
        node->startLine = v; node->startIsDot = dot;
        if (match(TokenType::Minus)) {
            int v2 = 0; bool dot2 = false;
            if (parseLineRefHere(v2, dot2)) {
                node->endLine = v2; node->endIsDot = dot2;
            }
        } else {
            node->endLine = v; node->endIsDot = dot;
        }
    }
    node->toPrinter = true;
    return node;
}

} // namespace gwbasic
