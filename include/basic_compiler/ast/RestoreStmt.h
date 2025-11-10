// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_RESTORESTMT_H
#define BASIC_COMPILER_AST_RESTORESTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: RestoreStmt
 * Purpose:
 *  - RESTORE — reset DATA read pointer to the start.
 * Inputs: none
 * Outputs:
 *  - Concrete Stmt node; codegen/semantics treat as control of data index
 */
struct RestoreStmt final : ASTLeaf<NodeKind::RestoreStmt, Stmt> {
    RestoreStmt() = default;
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_RESTORESTMT_H
