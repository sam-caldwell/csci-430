// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/BloadStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseBload
 * Purpose:
 *  - Parse BLOAD "filename"[,offset]
 * Inputs:
 *  - none (assumes 'BLOAD' was matched by caller)
 * Outputs:
 *  - BloadStmt: filename expression and optional numeric offset
 */
std::unique_ptr<Stmt> Parser::parseBload() {
    if (!check(TokenType::String)) throw ParseError("Expected filename string after BLOAD");
    auto fn = parsePrimary(); // StringExpr
    std::unique_ptr<Expr> off;
    if (match(TokenType::Comma)) off = parseExpression();
    return make_node<BloadStmt>({0,0}, std::move(fn), std::move(off));
}

} // namespace gwbasic

