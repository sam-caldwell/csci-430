// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/EnvironStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseEnviron
 * Summary:
 *  Parse ENVIRON statement assigning an environment variable string.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: EnvironStmt with string expression
 */
std::unique_ptr<Stmt> Parser::parseEnviron() {
    auto valueExpr = parseExpression();
    return make_node<EnvironStmt>({0,0}, std::move(valueExpr));
}

} // namespace gwbasic
