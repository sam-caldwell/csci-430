// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/RmdirStmt.h"
#include "basic_compiler/ast/make_node.h"

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
    auto p = parseExpression();
    return make_node<RmdirStmt>({0,0}, std::move(p));
}

} // namespace gwbasic
