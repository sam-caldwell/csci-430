// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ONGOSUBSTMT_H
#define BASIC_COMPILER_AST_ONGOSUBSTMT_H

#include <memory>
#include <vector>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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

#endif // BASIC_COMPILER_AST_ONGOSUBSTMT_H
