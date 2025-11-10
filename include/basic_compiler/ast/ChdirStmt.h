// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_CHDIRSTMT_H
#define BASIC_COMPILER_AST_CHDIRSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: ChdirStmt
 * Purpose:
 *  - CHDIR path — change working directory (unsafe file system effect).
 * Inputs:
 *  - path: String expression with the target directory
 * Outputs:
 *  - Concrete Stmt node; semantics validate string type and log unsafe usage
 */
struct ChdirStmt final : ASTLeaf<NodeKind::ChdirStmt, Stmt> {
    std::unique_ptr<Expr> path;
    explicit ChdirStmt(std::unique_ptr<Expr> path_expr)
        : path(std::move(path_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_CHDIRSTMT_H
