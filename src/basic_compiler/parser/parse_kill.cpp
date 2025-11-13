// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/KillStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseKill
 * Summary:
 *  Parse KILL statement: delete a file specified by an expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: KillStmt node with target expression
 */
std::unique_ptr<Stmt> Parser::parseKill() {
    auto fileExpr = parseExpression();
    return make_node<KillStmt>({0,0}, std::move(fileExpr));
}

} // namespace gwbasic
