// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ShellStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseShell
 * Summary:
 *  Parse SHELL statement optionally followed by a string expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ShellStmt with optional command expression
 */
std::unique_ptr<Stmt> Parser::parseShell() {
    // SHELL [string-expr]
    if (check(TokenType::NewLine) || check(TokenType::Colon)) {
        return make_node<ShellStmt>({0,0});
    }
    auto exprPtr = parseExpression();
    return make_node<ShellStmt>({0,0}, std::move(exprPtr));
}

} // namespace gwbasic
