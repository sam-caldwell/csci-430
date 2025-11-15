// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include <cctype>
#include <string>

namespace gwbasic {
    /*
     * Function: isStringExpr
     * Summary: Determine if an expression is string-typed for codegen decisions.
     * Parameters:
     *  - e: Expression pointer to inspect.
     * Returns:
     *  - bool: True if expression is known to be string.
     */
    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    bool CodeGenerator::isStringExpr(const Expr* expr) const {
        if (expr == nullptr) {
            return false;
        }

        if (isa<StringExpr>(expr)) {
            return true;
        }

        if (const auto* var = dyn_cast<const VarExpr>(expr)) {
            return (!var->name.empty() &&
                    var->name.back() == Symbols::DOLLARSIGN.first()) ||
                   semStringVariables_.contains(var->name);
        }

        if (const auto* bin = dyn_cast<const BinaryExpr>(expr)) {
            return (bin->op == BinaryOp::Add) &&
                   (isStringExpr(bin->lhs.get()) || isStringExpr(bin->rhs.get()));
        }

        if (const auto* call = dyn_cast<const CallExpr>(expr)) {
            // Built-in or user function with '$' suffix returns string
            if (!call->callee.empty() && call->callee.back() == Symbols::DOLLARSIGN.first()) {
                return true;
            }
            std::string funcNameUpper = call->callee;
            for (auto& chr : funcNameUpper) {
                chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
            }
            if (const auto iter = userFunctions_.find(funcNameUpper); iter != userFunctions_.end()) {
                const DefFnStmt *def = iter->second;
                return (!def->fnName.empty() &&
                        def->fnName.back() == Symbols::DOLLARSIGN.first());
            }
            return false;
        }
        return false;
    }
} // namespace gwbasic
