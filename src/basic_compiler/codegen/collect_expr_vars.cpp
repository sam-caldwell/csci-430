// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/VarExpr.h"

namespace gwbasic {

/*
 * Function: collectExprVars
 * Summary: Collect variable references in an expression tree.
 * Parameters:
 *  - expr: Expression node to analyze.
 * Returns:
 *  - void (updates internal variable set)
 */
void CodeGenerator::collectExprVars(const Expr* expr) {
    if (expr == nullptr) {
        return;
    }

    if (const auto* varExpr = dyn_cast<const VarExpr>(expr)) {
        variables_.insert(varExpr->name);
        logSem() << "VarRef " << varExpr->name << " @ " << varExpr->pos.line << ':' << varExpr->pos.col << Symbols::LF;
        return;
    }

    if (const auto* binExpr = dyn_cast<const BinaryExpr>(expr)) {
        collectExprVars(binExpr->lhs.get());
        collectExprVars(binExpr->rhs.get());
        return;
    }

    if (const auto* unaryExpr = dyn_cast<const UnaryExpr>(expr)) {
        collectExprVars(unaryExpr->inner.get());
        return;
    }

    if (const auto* callExpr = dyn_cast<const CallExpr>(expr)) {
        for (const auto& argNode : callExpr->args) {
            collectExprVars(argNode.get());
        }
        return;
    }
}

} // namespace gwbasic
