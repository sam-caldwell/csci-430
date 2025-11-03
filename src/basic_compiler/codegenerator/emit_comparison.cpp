// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitComparison
 * Inputs:
 *  - out: IR stream
 *  - c: BinaryExpr comparison node
 * Outputs:
 *  - std::string: name of the i1 result register
 * Theory of operation:
 *  - Emits code to evaluate both operands as double, then performs an
 *    IEEE-754 ordered comparison using the appropriate fcmp predicate.
 */
std::string CodeGenerator::emitComparison(std::ostringstream& out, const BinaryExpr* c) {
    // Support string vs string comparison via strcmp; otherwise numeric fcmp
    auto isStr = [&](const Expr* e, const auto& self) -> bool {
        if (isa<StringExpr>(e)) return true;
        if (const auto vv = dyn_cast<VarExpr>(e)) return !vv->name.empty() && vv->name.back() == CH_DOLLARSIGN;
        if (const auto bb = dyn_cast<BinaryExpr>(e)) return (bb->op == BinaryOp::Add) && (self(bb->lhs.get(), self) || self(bb->rhs.get(), self));
        return false;
    };
    const bool lhsIsStr = isStr(c->lhs.get(), isStr);
    if (const bool rhsIsStr = isStr(c->rhs.get(), isStr); lhsIsStr && rhsIsStr) {
        const auto ls = emitExpr(out, c->lhs.get(), "cmp");
        const auto rs = emitExpr(out, c->rhs.get(), "cmp");
        std::string call = nextTemp();
        {
            std::string ir = std::format("  {} = call i32 @strcmp(ptr {}, ptr {})", call, ls, rs);
            out << ir << STR_LF;
            log() << "line " << currentLine_ << " StrCmp -> " << ir << CH_LF;
        }
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
        {
            std::string rhs = "0"; //Simplified this from something I can't remember why I did it.
            std::string ir = std::format("  {} = icmp {} i32 {}, 0", res, pred, call);
            out << ir << STR_LF;
            log() << "line " << currentLine_ << " StrCmp icmp -> " << ir << CH_LF;
        }
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
        {
            std::string ir = std::format("  {} = fcmp {} double {}, {}", res, pred, lhsReg, rhsReg);
            out << ir << STR_LF;
            log() << "line " << currentLine_ << " Compare -> " << ir << CH_LF;
        }
        return res;
    }
}

} // namespace gwbasic
