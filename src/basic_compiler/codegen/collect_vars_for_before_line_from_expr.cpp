// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include <functional>
#include <set>
#include <string>

namespace gwbasic {

/*
 * Function: collectVarsForBeforeLineFromExpr
 * Summary: Collect scalar and array names referenced by an expression.
 * Parameters:
 *  - e: Expression node to scan.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array variable names.
 * Returns:
 *  - void
 */
void CodeGenerator::collectVarsForBeforeLineFromExpr(const Expr* expr,
                                                     std::set<std::string, std::less<>>& vars,
                                                     std::set<std::string, std::less<>>& arrays) {
    if (expr == nullptr) {
        return;
    }
    if (const auto* const var = dyn_cast<const VarExpr>(expr)) {
        vars.insert(var->name);
        return;
    }
    if (const auto* const call = dyn_cast<const CallExpr>(expr)) {
        if (arrayDims_.contains(call->callee)) {
            arrays.insert(call->callee);
        }
        for (const auto& arg : call->args) {
            collectVarsForBeforeLineFromExpr(arg.get(), vars, arrays);
        }
        return;
    }
    if (const auto* const binary = dyn_cast<const BinaryExpr>(expr)) {
        collectVarsForBeforeLineFromExpr(binary->lhs.get(), vars, arrays);
        collectVarsForBeforeLineFromExpr(binary->rhs.get(), vars, arrays);
        return;
    }
    if (const auto* const unary = dyn_cast<const UnaryExpr>(expr)) {
        collectVarsForBeforeLineFromExpr(unary->inner.get(), vars, arrays);
        return;
    }
}

} // namespace gwbasic
