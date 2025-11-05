// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: StopStmt
 * Purpose:
 *  - STOP: Halt program execution immediately (distinct from END).
 * Outputs:
 *  - Emits a break message and terminates program flow.
 */
struct StopStmt : ASTLeaf<NodeKind::StopStmt, Stmt> {
    StopStmt() : ASTLeaf() {}
};

} // namespace gwbasic

