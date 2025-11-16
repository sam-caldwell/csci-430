// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/codegen/CodeGenError.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitComparison
 * Summary: Lower a comparison BinaryExpr to i1 using fcmp/icmp.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - c: BinaryExpr comparison node.
 * Returns:
 *  - std::string: Name of the i1 result register.
 */
std::string CodeGenerator::emitComparison(std::ostringstream& out, const BinaryExpr* comp_expr) {
    // Support string vs string comparison via strcmp; otherwise numeric fcmp
    auto isStr = [&](const Expr* expr, const auto& self) -> bool {
        if (isa<StringExpr>(expr)) {
            return true;
        }
        if (const auto* const var_expr = dyn_cast<VarExpr>(expr)) {
            return !var_expr->name.empty() && var_expr->name.back() == Symbols::DOLLARSIGN.first();
        }
        if (const auto* const bin_expr = dyn_cast<BinaryExpr>(expr)) {
            return (bin_expr->op == BinaryOp::Add) && (self(bin_expr->lhs.get(), self) || self(bin_expr->rhs.get(), self));
        }
        return false;
    };
    const bool lhsIsStr = isStr(comp_expr->lhs.get(), isStr);
    if (const bool rhsIsStr = isStr(comp_expr->rhs.get(), isStr); lhsIsStr && rhsIsStr) {
        const auto lhs_str = emitExpr(out, comp_expr->lhs.get(), "cmp");
        const auto rhs_str = emitExpr(out, comp_expr->rhs.get(), "cmp");
        std::string call = nextTemp();
        out << std::format("  {} = call i32 @strcmp(ptr {}, ptr {})", call, lhs_str, rhs_str) << Symbols::LF;
        std::string res = nextTemp();
        const char* pred = nullptr;
        switch (comp_expr->op) {
            case BinaryOp::Eq: pred = "eq"; break;
            case BinaryOp::Ne: pred = "ne"; break;
            case BinaryOp::Lt: pred = "slt"; break;
            case BinaryOp::Le: pred = "sle"; break;
            case BinaryOp::Gt: pred = "sgt"; break;
            case BinaryOp::Ge: pred = "sge"; break;
            default: throw CodeGenError("Invalid comparison operator");
        }
        out << std::format("  {} = icmp {} i32 {}, 0", res, pred, call) << Symbols::LF;
        return res;
    }
    const auto lhsReg = emitExpr(out, comp_expr->lhs.get(), "cmp");
    const auto rhsReg = emitExpr(out, comp_expr->rhs.get(), "cmp");
    std::string res = nextTemp();
    const char* pred = nullptr;
    switch (comp_expr->op) {
        case BinaryOp::Eq: pred = "oeq"; break;
        case BinaryOp::Ne: pred = "one"; break;
        case BinaryOp::Lt: pred = "olt"; break;
        case BinaryOp::Le: pred = "ole"; break;
        case BinaryOp::Gt: pred = "ogt"; break;
        case BinaryOp::Ge: pred = "oge"; break;
        default: throw CodeGenError("Invalid comparison operator");
    }
    out << std::format("  {} = fcmp {} double {}, {}", res, pred, lhsReg, rhsReg) << Symbols::LF;
    return res;
}

} // namespace gwbasic
