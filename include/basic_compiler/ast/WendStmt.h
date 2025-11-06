// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: WendStmt (marker)
 * Purpose:
 *  - Structural marker for WEND terminating a WHILE block; removed during
 *    parseProgram() restructuring.
 * Inputs: none
 * Outputs:
 *  - Temporary Stmt, not present after folding
 */
struct WendStmt final : ASTLeaf<NodeKind::WendStmt, Stmt> { WendStmt() = default; };

} // namespace gwbasic
