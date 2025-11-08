// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/** ShellStmt: SHELL [command$] */
struct ShellStmt final : ASTLeaf<NodeKind::ShellStmt, Stmt> {
    std::unique_ptr<Expr> command; // optional
    ShellStmt() = default;
    explicit ShellStmt(std::unique_ptr<Expr> c) : ASTLeaf(), command(std::move(c)) {}
};

} // namespace gwbasic

