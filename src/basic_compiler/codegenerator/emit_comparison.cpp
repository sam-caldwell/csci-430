// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>

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
std::string CodeGenerator::emitComparison(std::ostringstream& out, const BinaryExpr* c) {
    // Support string vs string comparison via strcmp; otherwise numeric fcmp
    auto isStr = [&](const Expr* e, const auto& self) -> bool {
        if (isa<StringExpr>(e)) return true;
        if (const auto vv = dyn_cast<VarExpr>(e)) return !vv->name.empty() && vv->name.back() == Symbols::DOLLARSIGN.first();
        if (const auto bb = dyn_cast<BinaryExpr>(e)) return (bb->op == BinaryOp::Add) && (self(bb->lhs.get(), self) || self(bb->rhs.get(), self));
        return false;
    };
    const bool lhsIsStr = isStr(c->lhs.get(), isStr);
    if (const bool rhsIsStr = isStr(c->rhs.get(), isStr); lhsIsStr && rhsIsStr) {
        const auto ls = emitExpr(out, c->lhs.get(), "cmp");
        const auto rs = emitExpr(out, c->rhs.get(), "cmp");
        std::string call = nextTemp();
        out << std::format("  {} = call i32 @strcmp(ptr {}, ptr {})", call, ls, rs) << Symbols::LF;
        log() << "line " << currentLine_ << " StrCmp -> call strcmp" << Symbols::LF;
        std::string res = nextTemp();
        const char* pred = nullptr;
        switch (c->op) {
            case BinaryOp::Eq: pred = "eq"; break;
            case BinaryOp::Ne: pred = "ne"; break;
            case BinaryOp::Lt: pred = "slt"; break;
            case BinaryOp::Le: pred = "sle"; break;
            case BinaryOp::Gt: pred = "sgt"; break;
            case BinaryOp::Ge: pred = "sge"; break;
            default: throw CodeGenError("Invalid comparison operator");
        }
        out << std::format("  {} = icmp {} i32 {}, 0", res, pred, call) << Symbols::LF;
        log() << "line " << currentLine_ << " StrCmp icmp" << Symbols::LF;
        return res;
    } else {
        const auto lhsReg = emitExpr(out, c->lhs.get(), "cmp");
        const auto rhsReg = emitExpr(out, c->rhs.get(), "cmp");
        std::string res = nextTemp();
        const char* pred = nullptr;
        switch (c->op) {
            case BinaryOp::Eq: pred = "oeq"; break;
            case BinaryOp::Ne: pred = "one"; break;
            case BinaryOp::Lt: pred = "olt"; break;
            case BinaryOp::Le: pred = "ole"; break;
            case BinaryOp::Gt: pred = "ogt"; break;
            case BinaryOp::Ge: pred = "oge"; break;
            default: throw CodeGenError("Invalid comparison operator");
        }
        out << std::format("  {} = fcmp {} double {}, {}", res, pred, lhsReg, rhsReg) << Symbols::LF;
        log() << "line " << currentLine_ << " Compare fcmp" << Symbols::LF;
        return res;
    }
}

} // namespace gwbasic
