// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/CallAbsStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseCallAbs
 * Purpose:
 *  - Parse CALL address (absolute address)
 * Inputs:
 *  - none (assumes 'CALL' was matched by caller)
 * Outputs:
 *  - CallAbsStmt: numeric address expression
 */
std::unique_ptr<Stmt> Parser::parseCallAbs() {
    auto addr = parseExpression();
    return make_node<CallAbsStmt>({0,0}, std::move(addr));
}

} // namespace gwbasic

