// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ChdirStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseChdir
 * Summary:
 *  Parse CHDIR <string-expr> into a ChdirStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ChdirStmt with directory path expression
 */
std::unique_ptr<Stmt> Parser::parseChdir() {
    // CHDIR <string-expr>
    auto pathExpr = parseExpression();
    return make_node<ChdirStmt>({0,0}, std::move(pathExpr));
}

} // namespace gwbasic
