// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/CommonStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseCommon() {
    /*
     * Function: Parser::parseCommon
     * Inputs: none (assumes COMMON already consumed)
     * Outputs: CommonStmt with list of identifiers
     * Theory: Parse a non-empty comma-separated list of identifiers. Arrays
     *         and typed suffixes are not supported in this compiler.
     */
    std::vector<std::string> names;
    if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after COMMON");
    names.push_back(peek().lexeme);
    advance();
    while (match(TokenType::Comma)) {
        if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after comma in COMMON");
        names.push_back(peek().lexeme);
        advance();
    }
    return make_node<CommonStmt>({peek().line, peek().col}, std::move(names));
}

} // namespace gwbasic

