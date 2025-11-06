// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ErrorStmt.h"

namespace gwbasic {

/* Parse ERROR numeric-expression */
std::unique_ptr<Stmt> Parser::parseError() {
    auto e = parseExpression();
    return make_node<ErrorStmt>({0,0}, std::move(e));
}

} // namespace gwbasic

