// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: GosubStmt
 * Purpose:
 *  - Transfer control to a subroutine at a target line (paired with RETURN).
 * Inputs:
 *  - targetLine: Destination subroutine line number
 * Outputs:
 *  - Concrete Stmt node; codegen emits inline expansion with return label
 * Theory of operation:
 *  - Current lowering may inline GOSUB bodies and synthesize a return path.
 */
struct GosubStmt : ASTLeaf<NodeKind::GosubStmt, Stmt> {
    int targetLine;
    explicit GosubStmt(const int ln) : ASTLeaf(), targetLine(ln) {}
};

} // namespace gwbasic
