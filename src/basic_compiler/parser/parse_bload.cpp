// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/BloadStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseBload
 * Summary:
 *  Parse BLOAD "filename"[,offset] into a BloadStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: BloadStmt with filename and optional offset
 */
std::unique_ptr<Stmt> Parser::parseBload() {
    if (!check(TokenType::String)) {
        throw ParseError("Expected filename string after BLOAD");
    }
    auto fileExpr = parsePrimary(); // StringExpr
    std::unique_ptr<Expr> offsetExpr;
    if (match(TokenType::Comma)) {
        offsetExpr = parseExpression();
    }
    return make_node<BloadStmt>({0,0}, std::move(fileExpr), std::move(offsetExpr));
}

} // namespace gwbasic
