// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

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
    explicit DefSegStmt(std::unique_ptr<Expr> v) : ASTLeaf(), value(std::move(v)) {}
};

} // namespace gwbasic
