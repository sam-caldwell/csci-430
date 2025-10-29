// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/BinaryOp.h"

namespace gwbasic {

/**
 * Type: BinaryExpr
 * Purpose:
 *  - Represent a binary operation with two operand expressions.
 * Inputs:
 *  - op: Operator kind (arithmetic or comparison)
 *  - lhs, rhs: Owned operand expressions
 * Outputs:
 *  - Concrete Expr node; comparisons produce boolean-like numeric values
 * Theory of operation:
 *  - Codegen emits arithmetic as fadd/fsub/fmul/fdiv and comparisons as
 *    fcmp with subsequent uitofp to produce 0.0/1.0 semantics.
 */
struct BinaryExpr : Expr {
    BinaryOp op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    BinaryExpr(BinaryOp o, std::unique_ptr<Expr> a, std::unique_ptr<Expr> b)
        : op(o), lhs(std::move(a)), rhs(std::move(b)) {}
};

} // namespace gwbasic
