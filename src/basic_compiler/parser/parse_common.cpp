// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/CommonStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parseCommon
 * Summary:
 *  Parse COMMON var[,var...] and return a CommonStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: CommonStmt containing variable names
 */
std::unique_ptr<Stmt> Parser::parseCommon() {
    std::vector<std::string> names;
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected variable name after COMMON");
    }
    names.push_back(peek().lexeme);
    advance();
    while (match(TokenType::Comma)) {
        if (!check(TokenType::Identifier)) {
            throw ParseError("Expected variable name after comma in COMMON");
        }
        names.push_back(peek().lexeme);
        advance();
    }
    return make_node<CommonStmt>({peek().line, peek().col}, std::move(names));
}

} // namespace gwbasic
