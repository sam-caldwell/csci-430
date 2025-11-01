// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"

namespace gwbasic {

bool SemanticAnalyzer::constEval(const Expr* e, double& out) const {
    if (!e) return false;
    if (auto n = dyn_cast<const NumberExpr>(e)) { out = n->value; return true; }
    if (auto u = dyn_cast<const UnaryExpr>(e)) {
        double v;
        if (constEval(u->inner.get(), v)) {
            if (u->op == '+') { out = v; return true; }
            if (u->op == '-') { out = -v; return true; }
        }
        return false;
    }
    if (const auto b = dyn_cast<const BinaryExpr>(e)) {
        if (double L, R; constEval(b->lhs.get(), L) && constEval(b->rhs.get(), R)) {
            switch (b->op) {
                case BinaryOp::Add: out = L + R; return true;
                case BinaryOp::Sub: out = L - R; return true;
                case BinaryOp::Mul: out = L * R; return true;
                case BinaryOp::Div: out = L / R; return true;
                case BinaryOp::Eq:  out = (L == R) ? 1.0 : 0.0; return true;
                case BinaryOp::Ne:  out = (L != R) ? 1.0 : 0.0; return true;
                case BinaryOp::Lt:  out = (L <  R) ? 1.0 : 0.0; return true;
                case BinaryOp::Le:  out = (L <= R) ? 1.0 : 0.0; return true;
                case BinaryOp::Gt:  out = (L >  R) ? 1.0 : 0.0; return true;
                case BinaryOp::Ge:  out = (L >= R) ? 1.0 : 0.0; return true;
                default: return false;
            }
        }
        return false;
    }
    return false;
}

} // namespace gwbasic
