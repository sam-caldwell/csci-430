// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: PrintStmt
 * Purpose:
 *  - PRINT a string or numeric expression using printf.
 * Inputs:
 *  - value: Expression to print (StringExpr or numeric Expr)
 * Outputs:
 *  - Concrete Stmt node; codegen routes to @printf with proper format
 * Theory of operation:
 *  - String literals use "%s\n"; numeric expressions use "%f\n".
 */
struct PrintStmt : ASTLeaf<NodeKind::PrintStmt, Stmt> {
    std::unique_ptr<Expr> value; // may be StringExpr or other Expr
    explicit PrintStmt(std::unique_ptr<Expr> v) : ASTLeaf(), value(std::move(v)) {}
};

} // namespace gwbasic
