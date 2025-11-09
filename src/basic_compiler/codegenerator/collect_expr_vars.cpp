// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"

namespace gwbasic {

/*
 * Function: collectExprVars
 * Summary: Collect variable references in an expression tree.
 * Parameters:
 *  - e: Expression node to analyze.
 * Returns:
 *  - void (updates internal variable set)
 */
void CodeGenerator::collectExprVars(const Expr* e) {

    if (!e) return;

    if (const auto v = dyn_cast<const VarExpr>(e)) {
        variables_.insert(v->name);
        logSem() << "VarRef " << v->name << " @ " << v->pos.line << ':' << v->pos.col << Symbols::LF;
        return;
    }

    if (const auto b = dyn_cast<const BinaryExpr>(e)) {
        collectExprVars(b->lhs.get()); collectExprVars(b->rhs.get());
        return;
    }

    if (const auto u = dyn_cast<const UnaryExpr>(e)) {
        collectExprVars(u->inner.get());
        return;
    }

    if (const auto c = dyn_cast<const CallExpr>(e)) {
        for (const auto& a : c->args)
            collectExprVars(a.get());
        return;
    }
}

} // namespace gwbasic
