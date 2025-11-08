// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/** EnvironStmt: ENVIRON stringexpr ("NAME=VALUE") */
struct EnvironStmt final : ASTLeaf<NodeKind::EnvironStmt, Stmt> {
    std::unique_ptr<Expr> spec;
    explicit EnvironStmt(std::unique_ptr<Expr> s) : ASTLeaf(), spec(std::move(s)) {}
};

} // namespace gwbasic

