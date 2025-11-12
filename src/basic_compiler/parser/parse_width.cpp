// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/WidthStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseWidth
 * Summary:
 *  Parse WIDTH [device$,] columns according to EBNF.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: WidthStmt with device and columns expressions
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
