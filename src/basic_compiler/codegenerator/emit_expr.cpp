// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

namespace gwbasic {

std::string CodeGenerator::emitExpr(std::ostringstream& out, const Expr* e, const std::string&) {
    /*
     * Function: CodeGenerator::emitExpr
     * Inputs:
     *  - out: IR output stream
     *  - e: expression node
     *  - (unused) block suffix for naming (reserved)
     * Outputs:
     *  - std::string: register name or immediate literal used as the result
     * Theory of operation:
     *  - Pattern matches the expression type (number, var, unary, binary,
     *    string) and emits the corresponding LLVM IR instructions, returning
     *    a name/literal which the caller can use.
     */
    if (auto num = dynamic_cast<const NumberExpr*>(e)) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%.17g", num->value);
        std::string s(buf);
        if (s.find('.') == std::string::npos && s.find('e') == std::string::npos && s.find('E') == std::string::npos)
            s += ".0";

        return s;
    }
    if (auto v = dynamic_cast<const VarExpr*>(const_cast<Expr*>(e))) {
        ensureVarAllocated(out, v->name);
        std::string a = varAllocaName_[v->name];
        std::string r = nextTemp();
        {
            std::string ir = "  "; ir += r; ir += " = load double, ptr "; ir += a;
            out << ir << "\n";
            std::ostringstream m; m << "line " << currentLine_ << " VarExpr(" << v->name << ") -> " << ir; log(m.str());
        }
        return r;
    }
    if (auto u = dynamic_cast<const UnaryExpr*>(const_cast<Expr*>(e))) {
        auto inner = emitExpr(out, u->inner.get(), "");
        if (u->op == '+') return inner;
        if (u->op == '-') {
            std::string res = nextTemp();
            std::string ir = "  "; ir += res; ir += " = fsub double 0.0, "; ir += inner;
            out << ir << "\n";
            std::ostringstream m; m << "line " << currentLine_ << " UnaryExpr(-) -> " << ir; log(m.str());
            return res;
        }
    }
    if (auto b = dynamic_cast<const BinaryExpr*>(const_cast<Expr*>(e))) {
        if (b->op == BinaryOp::Eq || b->op == BinaryOp::Ne || b->op == BinaryOp::Lt || b->op == BinaryOp::Le || b->op == BinaryOp::Gt || b->op == BinaryOp::Ge) {
            std::string i1 = emitComparison(out, b);
            std::string i1z = nextTemp();
            {
                std::string ir = "  "; ir += i1z; ir += " = uitofp i1 "; ir += i1; ir += " to double";
                out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(cmp) -> " << ir; log(m.str()); }
            }
            return i1z;
        }
        auto L = emitExpr(out, b->lhs.get(), "");
        auto R = emitExpr(out, b->rhs.get(), "");
        std::string res = nextTemp();
        switch (b->op) {
            case BinaryOp::Add: { std::string ir = "  "; ir += res; ir += " = fadd double "; ir += L; ir += ", "; ir += R; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(+) -> " << ir; log(m.str()); } break; }
            case BinaryOp::Sub: { std::string ir = "  "; ir += res; ir += " = fsub double "; ir += L; ir += ", "; ir += R; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(-) -> " << ir; log(m.str()); } break; }
            case BinaryOp::Mul: { std::string ir = "  "; ir += res; ir += " = fmul double "; ir += L; ir += ", "; ir += R; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(*) -> " << ir; log(m.str()); } break; }
            case BinaryOp::Div: { std::string ir = "  "; ir += res; ir += " = fdiv double "; ir += L; ir += ", "; ir += R; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(/) -> " << ir; log(m.str()); } break; }
            default: throw CodeGenError("Unsupported binary op in arithmetic");
        }
        return res;
    }
    if (auto s = dynamic_cast<const StringExpr*>(const_cast<Expr*>(e))) {
        int id = strLiteralId_[s->value];
        std::string gep = nextTemp();
        std::string ir = "  "; ir += gep; ir += " = getelementptr inbounds i8, ptr "; ir += globalStringName(id); ir += ", i64 0";
        out << ir << "\n";
        std::ostringstream m; m << "line " << currentLine_ << " StringExpr -> " << ir; log(m.str());
        return gep;
    }
    throw CodeGenError("Unknown expression kind");
}

} // namespace gwbasic
