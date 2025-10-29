// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

void CodeGenerator::collectExprVars(const Expr* e) {
    /*
     * Function: CodeGenerator::collectExprVars
     * Inputs:
     *  - e: expression node to analyze
     * Outputs:
     *  - void (updates internal variable set)
     * Theory of operation:
     *  - Recursively visits the expression tree, recording any variable
     *    references for later allocation in the entry block.
     */
    if (!e) return;
    if (const auto v = dynamic_cast<const VarExpr*>(e)) {
        variables_.insert(v->name);
        std::ostringstream m; m << "VarRef " << v->name << " @ " << v->pos.line << ':' << v->pos.col; logSem(m.str());
        return;
    }
    if (const auto b = dynamic_cast<const BinaryExpr*>(e)) {
        collectExprVars(b->lhs.get()); collectExprVars(b->rhs.get()); return;
    }
    if (const auto u = dynamic_cast<const UnaryExpr*>(e)) {
        collectExprVars(u->inner.get()); return;
    }
}

} // namespace gwbasic
