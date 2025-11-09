// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/EraseStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseErase
 * Summary:
 *  Parse ERASE name[,name...] into an EraseStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: EraseStmt with array names to erase
 */
std::unique_ptr<Stmt> Parser::parseErase() {
    std::vector<std::string> names;
    if (!check(TokenType::Identifier)) throw ParseError("Expected array name after ERASE");
    names.emplace_back(peek().lexeme);
    advance();
    while (match(TokenType::Comma)) {
        if (!check(TokenType::Identifier)) throw ParseError("Expected array name after comma in ERASE");
        names.emplace_back(peek().lexeme);
        advance();
    }
    return make_node<EraseStmt>({peek().line, peek().col}, std::move(names));
}

} // namespace gwbasic
