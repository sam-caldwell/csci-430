// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"

namespace gwbasic {

/**
 * Type: IfStmt
 * Purpose:
 *  - Conditional branch to a given line if the comparison evaluates “true”.
 * Inputs:
 *  - cond: Comparison expression (BinaryExpr using comparison op)
 *  - targetLine: Line to branch to when cond is true (non-zero)
 * Outputs:
 *  - Concrete Stmt node; codegen emits fcmp + br
 * Theory of operation:
 *  - Comparisons produce 0.0/1.0; codegen compares against 0.0 and branches.
 */
struct IfStmt : Stmt {
    std::unique_ptr<Expr> cond;
    int targetLine;
    IfStmt(std::unique_ptr<Expr> c, int ln) : Stmt(NodeKind::IfStmt), cond(std::move(c)), targetLine(ln) {}
    static bool classof(const Node* N) { return N && N->kind == NodeKind::IfStmt; }
};

} // namespace gwbasic
