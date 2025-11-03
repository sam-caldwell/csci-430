// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

struct ReadTarget {
    std::string name;                 // variable or array name
    std::unique_ptr<Expr> index;      // optional array index if not null
};

/**
 * Type: ReadStmt
 * Purpose:
 *  - READ var[, var...] optionally with array element targets.
 */
struct ReadStmt : ASTLeaf<NodeKind::ReadStmt, Stmt> {
    std::vector<ReadTarget> targets;
    ReadStmt() = default;
    explicit ReadStmt(std::vector<ReadTarget> t) : ASTLeaf(), targets(std::move(t)) {}
};

struct RestoreStmt : ASTLeaf<NodeKind::RestoreStmt, Stmt> {
    RestoreStmt() = default;
};

} // namespace gwbasic

