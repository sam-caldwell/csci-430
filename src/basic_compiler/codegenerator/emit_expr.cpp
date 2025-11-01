// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <sstream>
#include <cctype>

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
    if (auto num = dyn_cast<const NumberExpr>(e)) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%.17g", num->value);
        std::string s(buf);
        if (s.find('.') == std::string::npos && s.find('e') == std::string::npos && s.find('E') == std::string::npos)
            s += ".0";

        return s;
    }
    if (auto v = dyn_cast<const VarExpr>(e)) {
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
    if (auto u = dyn_cast<const UnaryExpr>(e)) {
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
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
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
    if (auto call = dyn_cast<const CallExpr>(e)) {
        // Normalize function name to upper for matching
        std::string fn = call->callee; for (auto& ch : fn) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        // Emit args
        std::vector<std::string> argv; argv.reserve(call->args.size());
        for (const auto& a : call->args) argv.push_back(emitExpr(out, a.get(), ""));
        std::string res = nextTemp();
        if (fn == "SQR" || fn == "SQRT") {
            std::string ir = "  "; ir += res; ir += " = call double @sqrt(double "; ir += argv[0]; ir += ")";
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sqrt -> " << ir; log(m.str()); }
            return res;
        }
        if (fn == "ABS") {
            std::string ir = "  "; ir += res; ir += " = call double @fabs(double "; ir += argv[0]; ir += ")";
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fabs -> " << ir; log(m.str()); }
            return res;
        }
        if (fn == "SIN") { std::string ir = "  "; ir += res; ir += " = call double @sin(double "; ir += argv[0]; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sin -> " << ir; log(m.str()); } return res; }
        if (fn == "COS") { std::string ir = "  "; ir += res; ir += " = call double @cos(double "; ir += argv[0]; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr cos -> " << ir; log(m.str()); } return res; }
        if (fn == "TAN") { std::string ir = "  "; ir += res; ir += " = call double @tan(double "; ir += argv[0]; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr tan -> " << ir; log(m.str()); } return res; }
        if (fn == "ATN") { std::string ir = "  "; ir += res; ir += " = call double @atan(double "; ir += argv[0]; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr atan -> " << ir; log(m.str()); } return res; }
        if (fn == "LOG") { std::string ir = "  "; ir += res; ir += " = call double @log(double "; ir += argv[0]; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr log -> " << ir; log(m.str()); } return res; }
        if (fn == "EXP") { std::string ir = "  "; ir += res; ir += " = call double @exp(double "; ir += argv[0]; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr exp -> " << ir; log(m.str()); } return res; }
        if (fn == "INT") { std::string ir = "  "; ir += res; ir += " = call double @floor(double "; ir += argv[0]; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr floor(INT) -> " << ir; log(m.str()); } return res; }
        if (fn == "FIX") {
            std::string ireg = nextTemp();
            { std::string ir = "  "; ir += ireg; ir += " = fptosi double "; ir += argv[0]; ir += " to i64"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fix fptosi -> " << ir; log(m.str()); } }
            { std::string ir = "  "; ir += res; ir += " = sitofp i64 "; ir += ireg; ir += " to double"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fix sitofp -> " << ir; log(m.str()); } }
            return res;
        }
        if (fn == "SGN") {
            // sgn(x) = (x>0) - (x<0)
            std::string cmpPos = nextTemp();
            { std::string ir = "  "; ir += cmpPos; ir += " = fcmp ogt double "; ir += argv[0]; ir += ", 0.0"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn cmp>0 -> " << ir; log(m.str()); } }
            std::string posD = nextTemp();
            { std::string ir = "  "; ir += posD; ir += " = uitofp i1 "; ir += cmpPos; ir += " to double"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn pos to dbl -> " << ir; log(m.str()); } }
            std::string cmpNeg = nextTemp();
            { std::string ir = "  "; ir += cmpNeg; ir += " = fcmp olt double "; ir += argv[0]; ir += ", 0.0"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn cmp<0 -> " << ir; log(m.str()); } }
            std::string negD = nextTemp();
            { std::string ir = "  "; ir += negD; ir += " = uitofp i1 "; ir += cmpNeg; ir += " to double"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn neg to dbl -> " << ir; log(m.str()); } }
            std::string negVal = nextTemp();
            { std::string ir = "  "; ir += negVal; ir += " = fsub double 0.0, "; ir += negD; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn negate -> " << ir; log(m.str()); } }
            { std::string ir = "  "; ir += res; ir += " = fadd double "; ir += posD; ir += ", "; ir += negVal; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn add -> " << ir; log(m.str()); } }
            return res;
        }
        throw CodeGenError("Unknown function call");
    }
    if (auto s = dyn_cast<const StringExpr>(e)) {
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
