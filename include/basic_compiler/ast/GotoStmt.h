// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_GOTOSTMT_H
#define BASIC_COMPILER_AST_GOTOSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: GotoStmt
 * Purpose:
 *  - Unconditional branch to a specific line number.
 * Inputs:
 *  - targetLine: Destination program line
 * Outputs:
 *  - Concrete Stmt node; codegen emits an unconditional br to label
 * Theory of operation:
 *  - Line numbers are mapped to basic blocks with labels “line_<N>”.
 */
struct GotoStmt : ASTLeaf<NodeKind::GotoStmt, Stmt> {
    int targetLine;
    explicit GotoStmt(const int line_number) : targetLine(line_number) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_GOTOSTMT_H
