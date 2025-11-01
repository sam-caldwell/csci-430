// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/IfStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseIf() {
    /*
     * Function: Parser::parseIf
     * Inputs:
     *  - none (assumes IF already consumed)
     * Outputs:
     *  - IfStmt: conditional branch to a line on true
     * Theory of operation:
     *  - Parses a comparison expression, consumes THEN, reads a line number,
     *    and returns an IfStmt linking the condition and destination line.
     */
    auto cond = parseComparison();
    int l = peek().line, c = peek().col;
    consume(TokenType::KwThen, "THEN");
    if (!check(TokenType::Integer)) throw ParseError("Expected line number after THEN");
    int target = std::stoi(peek().lexeme);
    advance();
    return make_node<IfStmt>({l, c}, std::move(cond), target);
}

} // namespace gwbasic
