// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/BsaveStmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"

namespace gwbasic {

/*
 * Function: Parser::parseBsave
 * Summary:
 *  Parse BSAVE "filename", offset, length into a BsaveStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: BsaveStmt with filename, offset, length
 */
std::unique_ptr<Stmt> Parser::parseBsave() {
    if (!check(TokenType::String)) throw ParseError("Expected filename string after BSAVE");
    auto fn = parsePrimary();
    consume(TokenType::Comma, ",");
    auto off = parseExpression();
    consume(TokenType::Comma, ",");
    auto len = parseExpression();
    return make_node<BsaveStmt>({0,0}, std::move(fn), std::move(off), std::move(len));
}

} // namespace gwbasic
