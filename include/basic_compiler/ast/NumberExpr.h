// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_NUMBEREXPR_H
#define BASIC_COMPILER_AST_NUMBEREXPR_H

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: NumberExpr
 * Purpose:
 *  - Represent a numeric literal (double precision) in the AST.
 * Inputs:
 *  - value: Parsed literal value
 * Outputs:
 *  - Concrete Expr node used by codegen to materialize constants
 * Theory of operation:
 *  - Emitted as an SSA constant or loaded immediate in LLVM IR.
 */
struct NumberExpr : ASTLeaf<NodeKind::NumberExpr, Expr> {
    double value;
    explicit NumberExpr(const double v) : ASTLeaf(), value(v) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_NUMBEREXPR_H
