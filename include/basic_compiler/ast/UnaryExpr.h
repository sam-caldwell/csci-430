// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Expr.h"

namespace gwbasic {

/**
 * Type: UnaryExpr
 * Purpose:
 *  - Represent unary plus/minus applied to an expression.
 * Inputs:
 *  - op: '+' or '-'
 *  - inner: Owned operand expression
 * Outputs:
 *  - Concrete Expr node interpreted as +x or -x
 * Theory of operation:
 *  - Codegen emits a no-op for unary '+' and an fneg for unary '-'.
 */
struct UnaryExpr : Expr {
    char op; // '+' or '-'
    std::unique_ptr<Expr> inner;
    UnaryExpr(char o, std::unique_ptr<Expr> e) : Expr(NodeKind::UnaryExpr), op(o), inner(std::move(e)) {}
    static bool classof(const Node* N) { return N && N->kind == NodeKind::UnaryExpr; }
};

} // namespace gwbasic
