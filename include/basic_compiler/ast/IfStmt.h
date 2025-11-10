// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_IFSTMT_H
#define BASIC_COMPILER_AST_IFSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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
struct IfStmt : ASTLeaf<NodeKind::IfStmt, Stmt> {
    std::unique_ptr<Expr> cond;
    int targetLine;
    IfStmt(std::unique_ptr<Expr> cond_expr, const int line_number)
        : cond(std::move(cond_expr)), targetLine(line_number) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_IFSTMT_H
