// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/WidthStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseWidth
 * Purpose:
 *  - Parse WIDTH [device$,] columns according to EBNF.
 * Inputs:
 *  - none (assumes 'WIDTH' already matched by caller)
 * Outputs:
 *  - WidthStmt with optional device expression and required columns expression.
 */
std::unique_ptr<Stmt> Parser::parseWidth() {
    const int l = peek().line, c = peek().col;
    // Parse first expression; if followed by a ',', treat as device and parse columns.
    auto first = parseExpression();
    std::unique_ptr<Expr> device;
    std::unique_ptr<Expr> cols;
    if (match(TokenType::Comma)) {
        device = std::move(first);
        cols = parseExpression();
    } else {
        cols = std::move(first);
    }
    return make_node<WidthStmt>({l, c}, std::move(device), std::move(cols));
}

} // namespace gwbasic

