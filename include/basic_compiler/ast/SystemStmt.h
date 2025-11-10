// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_SYSTEMSTMT_H
#define BASIC_COMPILER_AST_SYSTEMSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: SystemStmt
 * Purpose:
 *  - SYSTEM: Terminate program and return to operating system.
 */
struct SystemStmt : ASTLeaf<NodeKind::SystemStmt, Stmt> {
    SystemStmt() : ASTLeaf() {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_SYSTEMSTMT_H
