// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/MergeStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseMerge
 * Purpose:
 *  - Parse MERGE "file"
 * Inputs:
 *  - none (assumes caller matched 'MERGE')
 * Outputs:
 *  - MergeStmt: filename string (as written)
 */
std::unique_ptr<Stmt> Parser::parseMerge() {
    if (!check(TokenType::String)) throw ParseError("Expected filename string after MERGE");
    std::string file = peek().lexeme; advance();
    return make_node<MergeStmt>({peek().line, peek().col}, std::move(file));
}

} // namespace gwbasic
