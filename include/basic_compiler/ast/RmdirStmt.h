// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_RMDIRSTMT_H
#define BASIC_COMPILER_AST_RMDIRSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/** RmdirStmt: RMDIR path$ */
struct RmdirStmt final : ASTLeaf<NodeKind::RmdirStmt, Stmt> {
    std::unique_ptr<Expr> path;
    explicit RmdirStmt(std::unique_ptr<Expr> path_expr)
        : path(std::move(path_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_RMDIRSTMT_H
