// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

/*
 * Function: Parser::parseExpression
 * Purpose:
 *  - Parse the top-level expression (lowest precedence).
 * Inputs:
 *  - none
 * Outputs:
 *  - Expr: expression node (delegates to parseComparison for precedence)
 */
std::unique_ptr<Expr> Parser::parseExpression() {
    /*
     * Function: Parser::parseExpression
     * Inputs:
     *  - none
     * Outputs:
     *  - Expr: top-level expression node
     * Theory of operation:
     *  - Delegates to parseComparison(), which handles precedence and
     *    relational operators.
     */
    return parseComparison();
}

} // namespace gwbasic
