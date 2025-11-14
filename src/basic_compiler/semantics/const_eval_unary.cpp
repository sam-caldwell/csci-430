// (c) 2025 Sam Caldwell. All Rights Reserved.
// NOLINTBEGIN(llvm-include-order)
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/compiler/Metrics.h"
// NOLINTEND(llvm-include-order)

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::constEvalUnary
 * Purpose:
 *  - Evaluate a unary expression when its inner expression is a constant.
 * Inputs:
 *  - u: Unary expression node
 *  - out: Reference to receive the numeric result
 * Outputs:
 *  - bool: true if a constant value was produced
 */
// NOLINTBEGIN(readability-identifier-length,readability-braces-around-statements,readability-implicit-bool-conversion)
bool SemanticAnalyzer::constEvalUnary(const UnaryExpr& unary, double& out) {
    double value = 0.0;
    if (!SemanticAnalyzer::constEval(unary.inner.get(), value)) { return false; }
    if (unary.op == Symbols::PLUS.first()) {
        if (gMetrics) { gMetrics->incUnaryElimPlus(); }
        out = value;
        return true;
    }
    if (unary.op == Symbols::MINUS.first()) {
        if (gMetrics) { gMetrics->incUnaryConstMinus(); }
        out = -value;
        return true;
    }
    return false;
}
// NOLINTEND(readability-identifier-length,readability-braces-around-statements,readability-implicit-bool-conversion)

}  // namespace gwbasic
