// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_MKDIRSTMT_H
#define BASIC_COMPILER_AST_MKDIRSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/** MkdirStmt: MKDIR path$ */
struct MkdirStmt final : ASTLeaf<NodeKind::MkdirStmt, Stmt> {
    std::unique_ptr<Expr> path;
    explicit MkdirStmt(std::unique_ptr<Expr> path_expr)
        : path(std::move(path_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_MKDIRSTMT_H
