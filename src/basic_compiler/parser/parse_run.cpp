// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/RunStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseRun
 * Inputs: none (assumes RUN already consumed)
 * Outputs: RunStmt with optional target line number
 * Theory: Accepts optional integer; otherwise restarts at first line.
 */
std::unique_ptr<Stmt> Parser::parseRun() {
    // Strict: require filename string; optional , <line>
    if (!check(TokenType::String)) throw ParseError("Expected filename string after RUN");
    std::string file = peek().lexeme; advance();
    int lineInfo = -1;
    if (match(TokenType::Comma)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected line number after comma in RUN");
        lineInfo = std::stoi(peek().lexeme); advance();
    }
    if (lineInfo >= 0) return make_node<RunStmt>({peek().line, peek().col}, file, lineInfo);
    return make_node<RunStmt>({peek().line, peek().col}, file, std::optional<int>{});
}

} // namespace gwbasic
