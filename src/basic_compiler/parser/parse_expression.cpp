// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/Expr.h"
#include <memory>

namespace gwbasic {

/*
 * Function: Parser::parseExpression
 * Summary:
 *  Parse the top-level expression and delegate to logical/comparison.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Expr>: Expression node
 */
std::unique_ptr<Expr> Parser::parseExpression() {
    return parseLogical();
}

} // namespace gwbasic
