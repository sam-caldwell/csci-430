// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_CLEARSTMT_H
#define BASIC_COMPILER_AST_CLEARSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: ClearStmt
 * Purpose:
 *  - CLEAR — reset program variables (and arrays) to default state.
 * Notes:
 *  - This compiler resets all numeric variables to 0.0 and zeroes any
 *    arrays that have been allocated so far.
 */
struct ClearStmt final : ASTLeaf<NodeKind::ClearStmt, Stmt> {
    ClearStmt() = default;
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_CLEARSTMT_H
