// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: SystemStmt
 * Purpose:
 *  - SYSTEM: Terminate program and return to operating system.
 */
struct SystemStmt : ASTLeaf<NodeKind::SystemStmt, Stmt> {
    SystemStmt() : ASTLeaf() {}
};

} // namespace gwbasic

