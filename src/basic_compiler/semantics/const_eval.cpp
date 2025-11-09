// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/UnaryExpr.h"

namespace gwbasic {

// Helpers are declared as private static methods on SemanticAnalyzer

/*
 * Function: SemanticAnalyzer::constEval
 * Inputs:
 *  - e: Expression to evaluate
 *  - out: Reference to receive constant value
 * Outputs:
 *  - bool: true if the expression evaluated to a constant number
 * Theory of operation:
 *  - Attempts constant folding for numbers, unary +/- and arithmetic/
 *    comparison binary operations recursively; returns false otherwise.
 */
bool SemanticAnalyzer::constEval(const Expr* expr, double& out) {
    if (expr == nullptr) {
        return false;
    }
    if (const auto* const num = dyn_cast<const NumberExpr>(expr)) {
        out = num->value;
        return true;
    }
    if (const auto* const unary = dyn_cast<const UnaryExpr>(expr)) {
        return SemanticAnalyzer::constEvalUnary(*unary, out);
    }
    if (const auto* const binary = dyn_cast<const BinaryExpr>(expr)) {
        return SemanticAnalyzer::constEvalBinary(*binary, out);
    }
    return false;
}

} // namespace gwbasic
