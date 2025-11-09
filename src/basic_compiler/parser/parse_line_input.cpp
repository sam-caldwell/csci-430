// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/LineInputStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>

namespace gwbasic {

/*
 * Function: Parser::parseLineInput
 * Summary:
 *  Parse LINE INPUT [#n,] var$ into a LineInputStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: LineInputStmt with channel and dest variable
 */
std::unique_ptr<Stmt> Parser::parseLineInput() {
    const int line = peek().line;
    const int col = peek().col;
    int channel = -1;
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) {
            throw ParseError("Expected channel number after '#'");
        }
        channel = std::stoi(peek().lexeme);
        advance();
        if (match(TokenType::Comma)) { /* optional comma */ }
    }
    // Optional leading ';' allowed (suppresses prompt spacing)
    if (match(TokenType::Semicolon)) { /* optional */ }
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected string variable after LINE INPUT");
    }
    const std::string name = peek().lexeme;
    advance();
    return make_node<LineInputStmt>({line, col}, channel, name);
}

} // namespace gwbasic
