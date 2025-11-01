// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include "basic_compiler/ast/Expr.h"

namespace gwbasic {

/**
 * Type: StringExpr
 * Purpose:
 *  - Represent a string literal in the AST (without surrounding quotes).
 * Inputs:
 *  - value: Raw string contents
 * Outputs:
 *  - Concrete Expr node used by codegen to place literal data in .rodata
 * Theory of operation:
 *  - Codegen interns literals and emits global string constants with
 *    references via getelementptr for @printf calls.
 */
struct StringExpr : Expr {
    std::string value;
    explicit StringExpr(std::string v) : Expr(NodeKind::StringExpr), value(std::move(v)) {}
    static bool classof(const Node* N) { return N && N->kind == NodeKind::StringExpr; }
};

} // namespace gwbasic
