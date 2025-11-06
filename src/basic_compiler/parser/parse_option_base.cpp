// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/OptionBaseStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseOptionBase
 * Purpose:
 *  - Parse OPTION BASE <Integer> (0 or 1)
 */
std::unique_ptr<Stmt> Parser::parseOptionBase() {
    // assumes 'OPTION' already matched; current should be 'BASE'
    consume(TokenType::KwBase, "BASE");
    if (!check(TokenType::Integer)) throw ParseError("Expected integer after OPTION BASE");
    int l = peek().line, c = peek().col;
    int base = std::stoi(peek().lexeme); advance();
    if (!(base == 0 || base == 1)) throw ParseError("OPTION BASE must be 0 or 1");
    return make_node<OptionBaseStmt>({l, c}, base);
}

} // namespace gwbasic

