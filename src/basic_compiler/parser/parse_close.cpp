// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/CloseStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseClose
 * Purpose:
 *  - Parse the CLOSE statement: CLOSE #<channel>
 * Inputs:
 *  - none (assumes 'CLOSE' was matched by caller)
 * Outputs:
 *  - CloseStmt: channel number to close
 */
std::unique_ptr<Stmt> Parser::parseClose() {
    int l = peek().line, c = peek().col;
    consume(TokenType::Hash, "#");
    if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
    int ch = std::stoi(peek().lexeme); advance();
    return make_node<CloseStmt>({l, c}, ch);
}

} // namespace gwbasic

