// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/RestoreStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseRestore
 * Summary:
 *  Parse RESTORE, which resets the DATA pointer.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: RestoreStmt marker node
 */
std::unique_ptr<Stmt> Parser::parseRestore() const {
    return make_node<RestoreStmt>({peek().line, peek().col});
}

} // namespace gwbasic
