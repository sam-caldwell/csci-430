// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/** KillStmt: KILL filespec$ */
struct KillStmt final : ASTLeaf<NodeKind::KillStmt, Stmt> {
    std::unique_ptr<Expr> filespec;
    explicit KillStmt(std::unique_ptr<Expr> f) : ASTLeaf(), filespec(std::move(f)) {}
};

} // namespace gwbasic

