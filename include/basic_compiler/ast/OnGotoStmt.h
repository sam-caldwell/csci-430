// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ONGOTOSTMT_H
#define BASIC_COMPILER_AST_ONGOTOSTMT_H

#include <memory>
#include <vector>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: OnGotoStmt
 * Purpose:
 *  - Dispatch by 1-based index to one of several GOTO line targets.
 * Inputs:
 *  - index: Numeric expression evaluated at runtime
 *  - targets: List of line numbers to branch to (1 => first, etc.)
 * Outputs:
 *  - Concrete Stmt node; codegen emits a switch with default fallthrough.
 */
struct OnGotoStmt : ASTLeaf<NodeKind::OnGotoStmt, Stmt> {
    std::unique_ptr<Expr> index;
    std::vector<int> targets;
    OnGotoStmt(std::unique_ptr<Expr> idx, std::vector<int> tgts)
        : ASTLeaf(), index(std::move(idx)), targets(std::move(tgts)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ONGOTOSTMT_H
