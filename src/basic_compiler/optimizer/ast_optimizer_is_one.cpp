// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_is_one.cpp
 * Purpose:
 *  - Define `AstOptimizer::isOne`, a helper to detect literal 1.0.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/Expr.h"

namespace gwbasic {

/**
 * Function: AstOptimizer::isOne
 * Purpose:
 *  - Determine if an expression is the constant numeric value 1.0.
 * Inputs:
 *  - e: Expression to probe (may be nullptr)
 * Outputs:
 *  - true if `e` is a `NumberExpr` equal to 1.0; false otherwise.
 */
bool AstOptimizer::isOne(const Expr* expr) {
    double value = 0.0;
    return asNumber(expr, value) && value == 1.0;
}

} // namespace gwbasic
