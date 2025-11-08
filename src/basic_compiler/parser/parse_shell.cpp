// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ShellStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseShell() {
    // SHELL [string-expr]
    if (check(TokenType::NewLine) || check(TokenType::Colon)) {
        return make_node<ShellStmt>({0,0});
    }
    auto e = parseExpression();
    return make_node<ShellStmt>({0,0}, std::move(e));
}

} // namespace gwbasic

