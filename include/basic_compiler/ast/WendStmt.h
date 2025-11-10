// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_WENDSTMT_H
#define BASIC_COMPILER_AST_WENDSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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

#endif // BASIC_COMPILER_AST_WENDSTMT_H
