// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_CLOSESTMT_H
#define BASIC_COMPILER_AST_CLOSESTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: CloseStmt
 * Purpose:
 *  - CLOSE #<n> — close an open file channel.
 * Inputs:
 *  - channel: Logical channel number to close (1..16)
 * Outputs:
 *  - Concrete Stmt node; semantics validate range
 */
struct CloseStmt final : ASTLeaf<NodeKind::CloseStmt, Stmt> {
    int channel{1};
    explicit CloseStmt(const int ch) : ASTLeaf(), channel(ch) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_CLOSESTMT_H
