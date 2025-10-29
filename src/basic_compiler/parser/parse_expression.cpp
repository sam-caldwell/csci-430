// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

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
