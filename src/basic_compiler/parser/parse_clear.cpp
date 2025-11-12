// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ClearStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseClear
 * Summary:
 *  Parse CLEAR (no arguments) into a ClearStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ClearStmt
 */
std::unique_ptr<Stmt> Parser::parseClear() const {
    return make_node<ClearStmt>({peek().line, peek().col});
}

} // namespace gwbasic
