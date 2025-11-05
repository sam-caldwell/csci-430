// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <vector>
#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: OnGosubStmt
 * Purpose:
 *  - Dispatch by 1-based index to one of several GOSUB line targets.
 * Inputs:
 *  - index: Numeric expression evaluated at runtime
 *  - targets: List of subroutine entry line numbers
 * Outputs:
 *  - Concrete Stmt node; codegen emits a switch with inline subroutines
 *    and a common continuation.
 */
struct OnGosubStmt : ASTLeaf<NodeKind::OnGosubStmt, Stmt> {
    std::unique_ptr<Expr> index;
    std::vector<int> targets;
    OnGosubStmt(std::unique_ptr<Expr> idx, std::vector<int> tgts)
        : ASTLeaf(), index(std::move(idx)), targets(std::move(tgts)) {}
};

} // namespace gwbasic

