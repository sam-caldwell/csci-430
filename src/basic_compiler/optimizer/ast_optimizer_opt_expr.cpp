// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_opt_expr.cpp
 * Purpose:
 *  - Define `AstOptimizer::optExpr`, the recursive expression simplifier
 *    that performs constant folding, unary simplifications, and algebraic
 *    identities.
 */
#include "basic_compiler/opt/AstOptimizer.h"

namespace gwbasic {

/**
 * Function: AstOptimizer::optExpr
 * Purpose:
 *  - Recursively rewrite an `Expr` to apply local simplifications and
 *    constant folding.
 * Inputs:
 *  - e: Owned expression to simplify (may be null)
 * Outputs:
 *  - Returns the (possibly replaced) simplified expression.
 * Details:
 *  - UnaryExpr: eliminates unary plus and folds unary minus for numbers.
 *  - BinaryExpr: folds arithmetic/comparisons; applies identities
 *    (x+0, x*1, x*0, x/1, etc.).
 */
std::unique_ptr<Expr> AstOptimizer::optExpr(std::unique_ptr<Expr> e) {
    if (!e) return e;
    if (auto u = dynamic_cast<UnaryExpr*>(e.get())) {
        u->inner = optExpr(std::move(u->inner));
        if (u->op == '+') return std::move(u->inner);
        if (u->op == '-') {
            double v; if (asNumber(u->inner.get(), v)) return std::make_unique<NumberExpr>(-v);
            return e;
        }
        return e;
    }
    if (auto b = dynamic_cast<BinaryExpr*>(e.get())) {
        b->lhs = optExpr(std::move(b->lhs));
        b->rhs = optExpr(std::move(b->rhs));
        double L, R;
        const bool lN = asNumber(b->lhs.get(), L);
        const bool rN = asNumber(b->rhs.get(), R);

        switch (b->op) {
            case BinaryOp::Add:
                if (lN && rN) return std::make_unique<NumberExpr>(L + R);
                if (isZero(b->lhs.get())) return std::move(b->rhs);
                if (isZero(b->rhs.get())) return std::move(b->lhs);
                return e;
            case BinaryOp::Sub:
                if (lN && rN) return std::make_unique<NumberExpr>(L - R);
                if (isZero(b->rhs.get())) return std::move(b->lhs);
                return e;
            case BinaryOp::Mul:
                if (lN && rN) return std::make_unique<NumberExpr>(L * R);
                if (isZero(b->lhs.get()) || isZero(b->rhs.get())) return std::make_unique<NumberExpr>(0.0);
                if (isOne(b->lhs.get())) return std::move(b->rhs);
                if (isOne(b->rhs.get())) return std::move(b->lhs);
                return e;
            case BinaryOp::Div:
                if (lN && rN) return std::make_unique<NumberExpr>(L / R);
                if (isOne(b->rhs.get())) return std::move(b->lhs);
                return e;
            case BinaryOp::Eq:
                if (lN && rN) return std::make_unique<NumberExpr>(L == R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Ne:
                if (lN && rN) return std::make_unique<NumberExpr>(L != R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Lt:
                if (lN && rN) return std::make_unique<NumberExpr>(L < R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Le:
                if (lN && rN) return std::make_unique<NumberExpr>(L <= R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Gt:
                if (lN && rN) return std::make_unique<NumberExpr>(L > R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Ge:
                if (lN && rN) return std::make_unique<NumberExpr>(L >= R ? 1.0 : 0.0);
                return e;
            default: return e;
        }
    }
    return e;
}

} // namespace gwbasic

