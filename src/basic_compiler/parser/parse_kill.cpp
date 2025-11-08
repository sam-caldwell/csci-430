// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/KillStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseKill() {
    auto f = parseExpression();
    return make_node<KillStmt>({0,0}, std::move(f));
}

} // namespace gwbasic

