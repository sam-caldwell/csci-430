// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_BINARYEXPR_H
#define BASIC_COMPILER_AST_BINARYEXPR_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"

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
struct BinaryExpr : ASTLeaf<NodeKind::BinaryExpr, Expr> {
    BinaryOp op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    BinaryExpr(const BinaryOp op_kind,
               std::unique_ptr<Expr> left_expr,
               std::unique_ptr<Expr> right_expr)
        : op(op_kind), lhs(std::move(left_expr)), rhs(std::move(right_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_BINARYEXPR_H
