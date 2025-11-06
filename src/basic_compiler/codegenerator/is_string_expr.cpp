// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include <cctype>

namespace gwbasic {

// Helper to determine if expression is string-typed for codegen decisions
bool CodeGenerator::isStringExpr(const Expr* e) const {
    if (!e) return false;
    if (isa<StringExpr>(e)) return true;
    if (auto v = dyn_cast<const VarExpr>(e)) return (!v->name.empty() && v->name.back() == Symbols::DOLLARSIGN.first()) || semStringVariables_.contains(v->name);
    if (auto b = dyn_cast<const BinaryExpr>(e)) return (b->op == BinaryOp::Add) && (isStringExpr(b->lhs.get()) || isStringExpr(b->rhs.get()));
    if (auto c = dyn_cast<const CallExpr>(e)) {
        // Built-in or user function with '$' suffix returns string
        if (!c->callee.empty() && c->callee.back() == Symbols::DOLLARSIGN.first()) return true;
        std::string fn = c->callee; for (auto &ch: fn) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        auto it = userFunctions_.find(fn);
        if (it != userFunctions_.end()) {
            const DefFnStmt* def = it->second;
            return (!def->fnName.empty() && def->fnName.back() == Symbols::DOLLARSIGN.first());
        }
        return false;
    }
    return false;
}

} // namespace gwbasic
