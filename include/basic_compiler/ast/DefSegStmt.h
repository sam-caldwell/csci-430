// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_DEFSEGSTMT_H
#define BASIC_COMPILER_AST_DEFSEGSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: DefSegStmt
 * Purpose:
 *  - DEF SEG [= expr] — set default memory segment for memory ops (no-op in this compiler).
 * Inputs:
 *  - value: Optional numeric segment expression (null = restore default)
 * Outputs:
 *  - Concrete Stmt node; semantics validate type when present
 */
struct DefSegStmt : ASTLeaf<NodeKind::DefSegStmt, Stmt> {
    std::unique_ptr<Expr> value; // null means restore default
    explicit DefSegStmt(std::unique_ptr<Expr> value_expr)
        : value(std::move(value_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_DEFSEGSTMT_H
