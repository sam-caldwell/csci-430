// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/RestoreStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseRestore
 * Purpose:
 *  - Parse the RESTORE statement (reset DATA pointer)
 * Inputs:
 *  - none (assumes 'RESTORE' was matched by caller)
 * Outputs:
 *  - RestoreStmt: marker node, no operands
 */
std::unique_ptr<Stmt> Parser::parseRestore() {
    return make_node<RestoreStmt>({peek().line, peek().col});
}

} // namespace gwbasic

