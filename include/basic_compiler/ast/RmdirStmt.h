// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/** RmdirStmt: RMDIR path$ */
struct RmdirStmt final : ASTLeaf<NodeKind::RmdirStmt, Stmt> {
    std::unique_ptr<Expr> path;
    explicit RmdirStmt(std::unique_ptr<Expr> p) : ASTLeaf(), path(std::move(p)) {}
};

} // namespace gwbasic

