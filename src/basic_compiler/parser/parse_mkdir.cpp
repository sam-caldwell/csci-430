// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/MkdirStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseMkdir() {
    auto p = parseExpression();
    return make_node<MkdirStmt>({0,0}, std::move(p));
}

} // namespace gwbasic

