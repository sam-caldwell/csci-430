// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::isComparisonExpr
 * Inputs:
 *  - e: Expression pointer to inspect
 * Outputs:
 *  - bool: true if 'e' is a BinaryExpr with a comparison operator
 * Theory of operation:
 *  - Checks for BinaryExpr and matches against Eq/Ne/Lt/Le/Gt/Ge ops.
 */
bool SemanticAnalyzer::isComparisonExpr(const Expr* e) const {
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
        switch (b->op) {
            case BinaryOp::Eq:
            case BinaryOp::Ne:
            case BinaryOp::Lt:
            case BinaryOp::Le:
            case BinaryOp::Gt:
            case BinaryOp::Ge:
                return true;
            default:
                return false;
        }
    }
    return false;
}

} // namespace gwbasic
