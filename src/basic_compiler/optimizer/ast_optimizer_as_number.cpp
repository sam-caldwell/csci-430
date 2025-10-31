// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_as_number.cpp
 * Purpose:
 *  - Define `AstOptimizer::asNumber`, a utility for recognizing numeric
 *    literals in expression trees and extracting their values.
 */
#include "basic_compiler/opt/AstOptimizer.h"

namespace gwbasic {

/**
 * Function: AstOptimizer::asNumber
 * Purpose:
 *  - Check whether an expression is a `NumberExpr` and extract its value.
 * Inputs:
 *  - e: Expression to probe (may be nullptr)
 *  - out: Reference to receive numeric value on success
 * Outputs:
 *  - Returns true if `e` is a `NumberExpr`; false otherwise. On success,
 *    `out` is assigned the contained number.
 */
bool AstOptimizer::asNumber(const Expr* e, double& out) {
    if (const auto n = dynamic_cast<const NumberExpr*>(e)) { out = n->value; return true; }
    return false;
}

} // namespace gwbasic

