// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/ReadTarget.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

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

} // namespace gwbasic
