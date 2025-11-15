// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

std::string CodeGenerator::emitIfCond(std::ostringstream &out, const Expr *condExpr) {
    // Accept comparison operators directly; otherwise treat non-zero as true.
    if (const auto *be = dyn_cast<const BinaryExpr>(condExpr)) {
        switch (be->op) {
            case BinaryOp::Eq:
            case BinaryOp::Ne:
            case BinaryOp::Lt:
            case BinaryOp::Le:
            case BinaryOp::Gt:
            case BinaryOp::Ge:
                return emitComparison(out, be);
            default:
                break;
        }
    }
    const std::string val = emitExpr(out, condExpr, "");
    const std::string cond = nextTemp();
    out << std::format("  {} = fcmp one double {}, 0.0", cond, val) << Symbols::LF;
    return cond;
}

void CodeGenerator::emitIfBranchHeader(std::ostringstream &out,
                                       const std::string &condReg,
                                       const std::string &thenLbl,
                                       const std::string &elseOrEndLbl) {
    const std::string br = std::format("  br i1 {}, label %{}, label %{}", condReg, thenLbl, elseOrEndLbl);
    out << br << Symbols::LF;
    log() << "line " << currentLine_ << " IfBlock -> " << br << Symbols::LF;
}

} // namespace gwbasic
