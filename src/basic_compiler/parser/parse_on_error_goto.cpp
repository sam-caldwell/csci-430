// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseOnErrorGoto
 * Inputs:
 *  - none (assumes KwOn already matched and the current token is KwError)
 * Outputs:
 *  - OnErrorGotoStmt with target line or 0 to disable
 * Grammar:
 *  - ON ERROR GOTO (0|line)
 */
std::unique_ptr<Stmt> Parser::parseOnErrorGoto() {
    consume(TokenType::KwError, "ERROR");
    consume(TokenType::KwGoto, "GOTO");
    if (!check(TokenType::Integer)) throw ParseError("Expected 0 or line number after ON ERROR GOTO");
    int ln = std::stoi(peek().lexeme);
    advance();
    return make_node<OnErrorGotoStmt>({0,0}, ln);
}

} // namespace gwbasic

