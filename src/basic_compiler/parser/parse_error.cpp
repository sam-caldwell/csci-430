// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ErrorStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseError
 * Summary:
 *  Parse ERROR statement followed by a numeric expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ErrorStmt with error code expression
 */
std::unique_ptr<Stmt> Parser::parseError() {
    auto e = parseExpression();
    return make_node<ErrorStmt>({0,0}, std::move(e));
}

} // namespace gwbasic
