// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/opt/AstOptimizer.h"

namespace gwbasic {

static inline std::unique_ptr<Expr> makeNumber(double v) {
    return std::make_unique<NumberExpr>(v);
}

bool AstOptimizer::asNumber(const Expr* e, double& out) const {
    if (const auto n = dynamic_cast<const NumberExpr*>(e)) { out = n->value; return true; }
    return false;
}

bool AstOptimizer::isZero(const Expr* e) const {
    double v; return asNumber(e, v) && v == 0.0;
}

bool AstOptimizer::isOne(const Expr* e) const {
    double v; return asNumber(e, v) && v == 1.0;
}

std::unique_ptr<Expr> AstOptimizer::optExpr(std::unique_ptr<Expr> e) {
    if (!e) return e;
    if (auto u = dynamic_cast<UnaryExpr*>(e.get())) {
        u->inner = optExpr(std::move(u->inner));
        if (u->op == '+') return std::move(u->inner);
        if (u->op == '-') {
            double v; if (asNumber(u->inner.get(), v)) return makeNumber(-v);
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

        // Constant folding: arithmetic
        switch (b->op) {
            case BinaryOp::Add:
                if (lN && rN) return makeNumber(L + R);
                if (isZero(b->lhs.get())) return std::move(b->rhs);
                if (isZero(b->rhs.get())) return std::move(b->lhs);
                return e;
            case BinaryOp::Sub:
                if (lN && rN) return makeNumber(L - R);
                if (isZero(b->rhs.get())) return std::move(b->lhs);
                return e;
            case BinaryOp::Mul:
                if (lN && rN) return makeNumber(L * R);
                if (isZero(b->lhs.get()) || isZero(b->rhs.get())) return makeNumber(0.0);
                if (isOne(b->lhs.get())) return std::move(b->rhs);
                if (isOne(b->rhs.get())) return std::move(b->lhs);
                return e;
            case BinaryOp::Div:
                if (lN && rN) return makeNumber(L / R);
                if (isOne(b->rhs.get())) return std::move(b->lhs);
                return e;
            case BinaryOp::Eq:
                if (lN && rN) return makeNumber(L == R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Ne:
                if (lN && rN) return makeNumber(L != R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Lt:
                if (lN && rN) return makeNumber(L < R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Le:
                if (lN && rN) return makeNumber(L <= R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Gt:
                if (lN && rN) return makeNumber(L > R ? 1.0 : 0.0);
                return e;
            case BinaryOp::Ge:
                if (lN && rN) return makeNumber(L >= R ? 1.0 : 0.0);
                return e;
            default: return e;
        }
    }
    // Leaf or unsupported: return as-is
    return e;
}

void AstOptimizer::optimize(Program& program) {
    for (auto& line : program.lines) {
        std::vector<std::unique_ptr<Stmt>> newStmts;
        newStmts.reserve(line.statements.size());
        for (auto& st : line.statements) {
            if (auto asg = dynamic_cast<AssignStmt*>(st.get())) {
                asg->value = optExpr(std::move(asg->value));
                newStmts.emplace_back(std::move(st));
            } else if (auto pr = dynamic_cast<PrintStmt*>(st.get())) {
                pr->value = optExpr(std::move(pr->value));
                newStmts.emplace_back(std::move(st));
            } else if (auto is = dynamic_cast<IfStmt*>(st.get())) {
                is->cond = optExpr(std::move(is->cond));
                double v;
                if (asNumber(is->cond.get(), v)) {
                    if (v != 0.0) {
                        // Replace with GOTO target
                        auto g = std::make_unique<GotoStmt>(is->targetLine);
                        g->pos = is->pos;
                        newStmts.emplace_back(std::move(g));
                    } else {
                        // Remove statement (no-op)
                    }
                } else {
                    newStmts.emplace_back(std::move(st));
                }
            } else if (auto fs = dynamic_cast<ForStmt*>(st.get())) {
                fs->start = optExpr(std::move(fs->start));
                fs->end   = optExpr(std::move(fs->end));
                if (fs->step) fs->step = optExpr(std::move(fs->step));
                // If step simplifies to 1.0, drop it to trigger default path in codegen
                if (fs->step && isOne(fs->step.get())) fs->step.reset();
                // Optimize body
                std::vector<std::unique_ptr<Stmt>> body;
                body.reserve(fs->body.size());
                for (auto& bs : fs->body) {
                    if (auto basg = dynamic_cast<AssignStmt*>(bs.get())) {
                        basg->value = optExpr(std::move(basg->value));
                        body.emplace_back(std::move(bs));
                    } else if (auto bpr = dynamic_cast<PrintStmt*>(bs.get())) {
                        bpr->value = optExpr(std::move(bpr->value));
                        body.emplace_back(std::move(bs));
                    } else {
                        // leave as-is; other constructs in FOR body unchanged
                        body.emplace_back(std::move(bs));
                    }
                }
                fs->body = std::move(body);
                newStmts.emplace_back(std::move(st));
            } else {
                // Other statements: GOTO/GOSUB/RETURN/END/INPUT left unchanged
                newStmts.emplace_back(std::move(st));
            }
        }
        line.statements = std::move(newStmts);
    }
}

} // namespace gwbasic

