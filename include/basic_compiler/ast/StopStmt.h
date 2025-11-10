// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_STOPSTMT_H
#define BASIC_COMPILER_AST_STOPSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: StopStmt
 * Purpose:
 *  - STOP: Halt program execution immediately (distinct from END).
 * Outputs:
 *  - Emits a break message and terminates program flow.
 */
struct StopStmt : ASTLeaf<NodeKind::StopStmt, Stmt> {
    StopStmt() = default;
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_STOPSTMT_H
