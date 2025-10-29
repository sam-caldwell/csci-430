// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Expr.h"

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
struct NumberExpr : Expr {
    double value;
    explicit NumberExpr(double v) : value(v) {}
};

} // namespace gwbasic
