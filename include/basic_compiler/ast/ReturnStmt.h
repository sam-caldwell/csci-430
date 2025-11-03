// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: ReturnStmt
 * Purpose:
 *  - Return from a subroutine invoked via GOSUB.
 * Inputs: none
 * Outputs:
 *  - Concrete Stmt node; codegen branches to synthesized return label
 * Theory of operation:
 *  - Works in conjunction with GosubStmt lowering strategy.
 */
struct ReturnStmt final : ASTLeaf<NodeKind::ReturnStmt, Stmt> {};

} // namespace gwbasic
