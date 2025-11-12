// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/MkdirStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseMkdir
 * Summary:
 *  Parse MKDIR statement with a single path expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: MkdirStmt with target path expression
 */
std::unique_ptr<Stmt> Parser::parseMkdir() {
    auto pathExpr = parseExpression();
    return make_node<MkdirStmt>({0,0}, std::move(pathExpr));
}

} // namespace gwbasic
