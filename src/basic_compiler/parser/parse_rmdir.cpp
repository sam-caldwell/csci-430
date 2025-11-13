// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/RmdirStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseRmdir
 * Summary:
 *  Parse RMDIR statement with a single path expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: RmdirStmt with target path expression
 */
std::unique_ptr<Stmt> Parser::parseRmdir() {
    auto pathExpr = parseExpression();
    return make_node<RmdirStmt>({0,0}, std::move(pathExpr));
}

} // namespace gwbasic
