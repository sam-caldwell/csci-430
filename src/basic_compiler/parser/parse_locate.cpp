// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/LocateStmt.h"

namespace gwbasic {

// LOCATE row[, col]
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

