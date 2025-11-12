// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/MergeStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>
#include <utility>

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
    if (!check(TokenType::String)) {
        throw ParseError("Expected filename string after MERGE");
    }
    std::string file = peek().lexeme;
    advance();
    return make_node<MergeStmt>({peek().line, peek().col}, std::move(file));
}

} // namespace gwbasic
