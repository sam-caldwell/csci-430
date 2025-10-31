// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_is_zero.cpp
 * Purpose:
 *  - Define `AstOptimizer::isZero`, a helper to detect literal 0.0.
 */
#include "basic_compiler/opt/AstOptimizer.h"

namespace gwbasic {

/**
 * Function: AstOptimizer::isZero
 * Purpose:
 *  - Determine if an expression is the constant numeric value 0.0.
 * Inputs:
 *  - e: Expression to probe (may be nullptr)
 * Outputs:
 *  - true if `e` is a `NumberExpr` equal to 0.0; false otherwise.
 */
bool AstOptimizer::isZero(const Expr* e) {
    double v; return asNumber(e, v) && v == 0.0;
}

} // namespace gwbasic

