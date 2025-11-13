// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/LocateStmt.h"
#include "basic_compiler/ast/SourcePos.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseLocate
 * Summary:
 *  Parse LOCATE row[,col] into a LocateStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: LocateStmt with row and optional column
 */
std::unique_ptr<Stmt> Parser::parseLocate() {
    auto start = peek();
    auto row = parseExpression();
    std::unique_ptr<Expr> col;
    if (match(TokenType::Comma)) {
        col = parseExpression();
    }
    return std::make_unique<LocateStmt>(SourcePos{start.line, start.col}, std::move(row), std::move(col));
}

} // namespace gwbasic
