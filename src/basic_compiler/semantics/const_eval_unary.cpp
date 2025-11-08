// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/compiler/Metrics.h"

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
bool SemanticAnalyzer::constEvalUnary(const UnaryExpr& u, double& out) {
    double v = 0.0;
    if (!SemanticAnalyzer::constEval(u.inner.get(), v)) return false;
    if (u.op == Symbols::PLUS.first()) {
        if (gMetrics) gMetrics->incUnaryElimPlus();
        out = v;
        return true;
    }
    if (u.op == Symbols::MINUS.first()) {
        if (gMetrics) gMetrics->incUnaryConstMinus();
        out = -v;
        return true;
    }
    return false;
}

}  // namespace gwbasic

