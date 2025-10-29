// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

namespace gwbasic {

std::string CodeGenerator::emitComparison(std::ostringstream& out, const BinaryExpr* c) {
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
        std::string ir = "  "; ir += res; ir += " = fcmp "; ir += pred; ir += " double "; ir += lhsReg; ir += ", "; ir += rhsReg;
        out << ir << "\n";
        std::ostringstream m; m << "line " << currentLine_ << " Compare -> " << ir; log(m.str());
    }
    return res;
}

} // namespace gwbasic
