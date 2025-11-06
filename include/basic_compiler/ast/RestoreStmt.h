// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: RestoreStmt
 * Purpose:
 *  - RESTORE — reset DATA read pointer to the start.
 * Inputs: none
 * Outputs:
 *  - Concrete Stmt node; codegen/semantics treat as control of data index
 */
struct RestoreStmt final : ASTLeaf<NodeKind::RestoreStmt, Stmt> {
    RestoreStmt() = default;
};

} // namespace gwbasic
