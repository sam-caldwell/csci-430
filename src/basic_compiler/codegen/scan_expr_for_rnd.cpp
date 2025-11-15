// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include <cctype>
#include <string>

namespace gwbasic {

/*
 * Function: scanExprForRnd
 * Summary: Scan an expression to detect RND usage.
 * Parameters:
 *  - e: Expression node to scan.
 * Returns:
 *  - void (sets internal flag when RND is referenced)
 */
void CodeGenerator::scanExprForRnd(const Expr* expr) {
    if (expr == nullptr) {
        return;
    }
    if (const auto* call = dyn_cast<const CallExpr>(expr)) {
        std::string funcNameUpper = call->callee;
        for (auto& chr : funcNameUpper) {
            chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
        }
        if (funcNameUpper == "RND") {
            needsRndHelper_ = true;
        }
        for (const auto& arg : call->args) {
            scanExprForRnd(arg.get());
        }
        return;
    }
    if (const auto* bin = dyn_cast<const BinaryExpr>(expr)) {
        scanExprForRnd(bin->lhs.get());
        scanExprForRnd(bin->rhs.get());
        return;
    }
    if (const auto* unary = dyn_cast<const UnaryExpr>(expr)) {
        scanExprForRnd(unary->inner.get());
    }
}

} // namespace gwbasic
