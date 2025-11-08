// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/LineInputStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseLineInput
 * Purpose:
 *  - Parse LINE INPUT [#n,] var$
 * Inputs:
 *  - none (assumes 'LINE INPUT' tokens were matched by caller)
 * Outputs:
 *  - LineInputStmt: optional channel and destination string variable name
 */
std::unique_ptr<Stmt> Parser::parseLineInput() {
    int l = peek().line, c = peek().col;
    int channel = -1;
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
        channel = std::stoi(peek().lexeme); advance();
        if (match(TokenType::Comma)) {}
    }
    // Optional leading ';' allowed (suppresses prompt spacing)
    if (match(TokenType::Semicolon)) { /* optional */ }
    if (!check(TokenType::Identifier)) throw ParseError("Expected string variable after LINE INPUT");
    std::string name = peek().lexeme; advance();
    return make_node<LineInputStmt>({l, c}, channel, name);
}

} // namespace gwbasic
