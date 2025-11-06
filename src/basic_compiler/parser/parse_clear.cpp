// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ClearStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseClear
 * Purpose:
 *  - Parse the CLEAR statement (no arguments).
 * Outputs:
 *  - ClearStmt
 */
std::unique_ptr<Stmt> Parser::parseClear() const {
    return make_node<ClearStmt>({peek().line, peek().col});
}

} // namespace gwbasic

