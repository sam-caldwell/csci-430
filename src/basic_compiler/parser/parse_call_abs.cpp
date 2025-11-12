// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/CallAbsStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseCallAbs
 * Summary:
 *  Parse CALL with an absolute address expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: CallAbsStmt with target address expression
 */
std::unique_ptr<Stmt> Parser::parseCallAbs() {
    auto addr = parseExpression();
    return make_node<CallAbsStmt>({0,0}, std::move(addr));
}

} // namespace gwbasic
