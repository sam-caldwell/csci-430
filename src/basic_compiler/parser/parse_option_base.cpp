// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/OptionBaseStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>

namespace gwbasic {

/*
 * Function: Parser::parseOptionBase
 * Summary:
 *  Parse OPTION BASE <Integer> (0 or 1) into an OptionBaseStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: OptionBaseStmt with base value
 */
std::unique_ptr<Stmt> Parser::parseOptionBase() {
    // assumes 'OPTION' already matched; current should be 'BASE'
    consume(TokenType::KwBase, "BASE");
    if (!check(TokenType::Integer)) {
        throw ParseError("Expected integer after OPTION BASE");
    }
    const int line = peek().line;
    const int col = peek().col;
    const int base = std::stoi(peek().lexeme);
    advance();
    if ((base != 0) && (base != 1)) {
        throw ParseError("OPTION BASE must be 0 or 1");
    }
    return make_node<OptionBaseStmt>({line, col}, base);
}

} // namespace gwbasic
