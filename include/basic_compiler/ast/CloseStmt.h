// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: CloseStmt
 * Purpose:
 *  - CLOSE #<n> — close an open file channel.
 * Inputs:
 *  - channel: Logical channel number to close (1..16)
 * Outputs:
 *  - Concrete Stmt node; semantics validate range
 */
struct CloseStmt final : ASTLeaf<NodeKind::CloseStmt, Stmt> {
    int channel{1};
    explicit CloseStmt(const int ch) : ASTLeaf(), channel(ch) {}
};

} // namespace gwbasic
