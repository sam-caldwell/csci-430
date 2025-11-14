// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/compiler/Metrics.h"

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::constEvalBinary
 * Purpose:
 *  - Evaluate a binary expression when both operands are constant.
 * Inputs:
 *  - b: Binary expression node
 *  - out: Reference to receive the numeric result
 * Outputs:
 *  - bool: true if a constant value was produced
 */
// NOLINTBEGIN(readability-function-cognitive-complexity,readability-function-size,readability-implicit-bool-conversion,readability-braces-around-statements,readability-identifier-length)
bool SemanticAnalyzer::constEvalBinary(const BinaryExpr& b, double& out) {
    double L = 0.0;
    double R = 0.0;
    if (!(SemanticAnalyzer::constEval(b.lhs.get(), L) && SemanticAnalyzer::constEval(b.rhs.get(), R))) {
        return false;
    }
    switch (b.op) {
        case BinaryOp::Add:
            out = L + R;
            if (gMetrics) gMetrics->incConstFoldAdd();
            return true;
        case BinaryOp::Sub:
            out = L - R;
            if (gMetrics) gMetrics->incConstFoldSub();
            return true;
        case BinaryOp::Mul:
            out = L * R;
            if (gMetrics) gMetrics->incConstFoldMul();
            return true;
        case BinaryOp::Div:
            out = L / R;
            if (gMetrics) gMetrics->incConstFoldDiv();
            return true;
        case BinaryOp::Eq:
            out = (L == R) ? 1.0 : 0.0;
            if (gMetrics) gMetrics->incConstFoldCmp();
            return true;
        case BinaryOp::Ne:
            out = (L != R) ? 1.0 : 0.0;
            if (gMetrics) gMetrics->incConstFoldCmp();
            return true;
        case BinaryOp::Lt:
            out = (L < R) ? 1.0 : 0.0;
            if (gMetrics) gMetrics->incConstFoldCmp();
            return true;
        case BinaryOp::Le:
            out = (L <= R) ? 1.0 : 0.0;
            if (gMetrics) gMetrics->incConstFoldCmp();
            return true;
        case BinaryOp::Gt:
            out = (L > R) ? 1.0 : 0.0;
            if (gMetrics) gMetrics->incConstFoldCmp();
            return true;
        case BinaryOp::Ge:
            out = (L >= R) ? 1.0 : 0.0;
            if (gMetrics) gMetrics->incConstFoldCmp();
            return true;
        default:
            return false;
    }
}

// NOLINTEND(readability-function-cognitive-complexity,readability-function-size,readability-implicit-bool-conversion,readability-braces-around-statements,readability-identifier-length)

}  // namespace gwbasic
