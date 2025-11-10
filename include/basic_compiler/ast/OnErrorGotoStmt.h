// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ONERRORGOTOSTMT_H
#define BASIC_COMPILER_AST_ONERRORGOTOSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: OnErrorGotoStmt
 * Purpose:
 *  - Establish or clear an error handler trap line.
 * Inputs:
 *  - targetLine: BASIC line number to branch to on error; 0 clears trap.
 */
struct OnErrorGotoStmt : ASTLeaf<NodeKind::OnErrorGotoStmt, Stmt> {
    int targetLine{0}; // 0 => disable
    explicit OnErrorGotoStmt(int target_line) : targetLine(target_line) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ONERRORGOTOSTMT_H
