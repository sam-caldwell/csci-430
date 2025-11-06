// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/compiler/Metrics.h"

namespace gwbasic {

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
bool SemanticAnalyzer::constEval(const Expr* e, double& out) {
    if (!e) return false;
    if (const auto n = dyn_cast<const NumberExpr>(e)) { out = n->value; return true; }
    if (const auto u = dyn_cast<const UnaryExpr>(e)) {
        if (double v; constEval(u->inner.get(), v)) {
            if (u->op == Symbols::PLUS.first()) { out = v; return true; }
            if (u->op == Symbols::MINUS.first()) { out = -v; return true; }
        }
        return false;
    }
    if (const auto b = dyn_cast<const BinaryExpr>(e)) {
        if (double L, R; constEval(b->lhs.get(), L) && constEval(b->rhs.get(), R)) {
            switch (b->op) {
                case BinaryOp::Add: out = L + R; if (gMetrics) gMetrics->incConstFoldAdd(); return true;
                case BinaryOp::Sub: out = L - R; if (gMetrics) gMetrics->incConstFoldSub(); return true;
                case BinaryOp::Mul: out = L * R; if (gMetrics) gMetrics->incConstFoldMul(); return true;
                case BinaryOp::Div: out = L / R; if (gMetrics) gMetrics->incConstFoldDiv(); return true;
                case BinaryOp::Eq:  out = (L == R) ? 1.0 : 0.0; if (gMetrics) gMetrics->incConstFoldCmp(); return true;
                case BinaryOp::Ne:  out = (L != R) ? 1.0 : 0.0; if (gMetrics) gMetrics->incConstFoldCmp(); return true;
                case BinaryOp::Lt:  out = (L <  R) ? 1.0 : 0.0; if (gMetrics) gMetrics->incConstFoldCmp(); return true;
                case BinaryOp::Le:  out = (L <= R) ? 1.0 : 0.0; if (gMetrics) gMetrics->incConstFoldCmp(); return true;
                case BinaryOp::Gt:  out = (L >  R) ? 1.0 : 0.0; if (gMetrics) gMetrics->incConstFoldCmp(); return true;
                case BinaryOp::Ge:  out = (L >= R) ? 1.0 : 0.0; if (gMetrics) gMetrics->incConstFoldCmp(); return true;
                default: return false;
            }
        }
        return false;
    }
    return false;
}

} // namespace gwbasic
