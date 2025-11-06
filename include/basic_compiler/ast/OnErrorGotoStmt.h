// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: OnErrorGotoStmt
 * Purpose:
 *  - Establish or clear an error handler trap line.
 * Inputs:
 *  - targetLine: BASIC line number to branch to on error; 0 clears trap.
 */
struct OnErrorGotoStmt : ASTLeaf<NodeKind::OnErrorGotoStmt, Stmt> {
    int targetLine{0}; // 0 => disable
    explicit OnErrorGotoStmt(int target) : ASTLeaf(), targetLine(target) {}
};

} // namespace gwbasic

