// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: ErrorStmt
 * Purpose:
 *  - Raise a runtime error with a numeric error code.
 * Inputs:
 *  - code: numeric expression evaluated to an integer error number.
 */
struct ErrorStmt : ASTLeaf<NodeKind::ErrorStmt, Stmt> {
    std::unique_ptr<Expr> code;
    explicit ErrorStmt(std::unique_ptr<Expr> c) : ASTLeaf(), code(std::move(c)) {}
};

} // namespace gwbasic

