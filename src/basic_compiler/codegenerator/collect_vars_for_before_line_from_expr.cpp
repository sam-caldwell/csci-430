// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"

namespace gwbasic {

void CodeGenerator::collectVarsForBeforeLineFromExpr(const Expr* e, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays) {
    if (!e) return;
    if (const auto v = dyn_cast<const VarExpr>(e)) {
        vars.insert(v->name);
        return;
    }
    if (const auto c = dyn_cast<const CallExpr>(e)) {
        // Array element reference syntax uses call-form: A(index[,index...])
        if (arrayDims_.contains(c->callee)) {
            arrays.insert(c->callee);
            for (const auto& a : c->args)
                collectVarsForBeforeLineFromExpr(a.get(), vars, arrays);
            return;
        }
        for (const auto& a : c->args)
            collectVarsForBeforeLineFromExpr(a.get(), vars, arrays);
        return;
    }
    if (const auto b = dyn_cast<const BinaryExpr>(e)) {
        collectVarsForBeforeLineFromExpr(b->lhs.get(), vars, arrays);
        collectVarsForBeforeLineFromExpr(b->rhs.get(), vars, arrays); return;
    }
    if (const auto u = dyn_cast<const UnaryExpr>(e)) {
        collectVarsForBeforeLineFromExpr(u->inner.get(), vars, arrays); return;
    }
}

} // namespace gwbasic
