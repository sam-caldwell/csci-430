// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ENDSTMT_H
#define BASIC_COMPILER_AST_ENDSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: EndStmt
 * Purpose:
 *  - Terminate program execution.
 * Inputs: none
 * Outputs:
 *  - Concrete Stmt node; codegen emits a return from main
 * Theory of operation:
 *  - Subsequent lines are not executed.
 */
struct EndStmt : ASTLeaf<NodeKind::EndStmt, Stmt> {
    EndStmt() = default;
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ENDSTMT_H
