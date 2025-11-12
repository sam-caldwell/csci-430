// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/CloseStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>

namespace gwbasic {

/*
 * Function: Parser::parseClose
 * Summary:
 *  Parse CLOSE #<channel> into a CloseStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: CloseStmt with channel number
 */
std::unique_ptr<Stmt> Parser::parseClose() {
    const int lineNum = peek().line;
    const int colNum = peek().col;
    consume(TokenType::Hash, "#");
    if (!check(TokenType::Integer)) {
        throw ParseError("Expected channel number after '#'");
    }
    const int channel = std::stoi(peek().lexeme);
    advance();
    return make_node<CloseStmt>({lineNum, colNum}, channel);
}

} // namespace gwbasic
