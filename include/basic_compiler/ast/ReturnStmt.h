// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_RETURNSTMT_H
#define BASIC_COMPILER_AST_RETURNSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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

#endif // BASIC_COMPILER_AST_RETURNSTMT_H
