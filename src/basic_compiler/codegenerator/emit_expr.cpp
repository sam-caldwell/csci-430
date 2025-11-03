// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>
#include <cctype>

namespace gwbasic {

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
std::string CodeGenerator::emitExpr(std::ostringstream& out, const Expr* e, [[maybe_unused]] const std::string& currBlockSuffix) {
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

        if (!v->name.empty() && v->name.back() == CH_DOLLARSIGN) {
            std::string ir = std::format("  {} = load ptr, ptr {}", r, a);
            out << ir << STR_LF;
            log() << "line " << currentLine_ << " VarExpr$ -> " << ir << CH_LF;
        } else {
            std::string ir = std::format("  {} = load double, ptr {}", r, a);
            out << ir << STR_LF;
            log() <<"line " << currentLine_ << " VarExpr -> " << ir << CH_LF;
        }
        return r;
    }
    if (auto u = dyn_cast<const UnaryExpr>(e)) {
        auto inner = emitExpr(out, u->inner.get(), "");
        if (u->op == '+') return inner;
        if (u->op == '-') {
            std::string res = nextTemp();
            std::string ir = std::format("  {} = fsub double 0.0, {}", res, inner);
            out << ir << STR_LF;
            std::ostringstream m; m << "line " << currentLine_ << " UnaryExpr(-) -> " << ir; log() << m.str() << CH_LF;
            return res;
        }
    }
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
        if (b->op == BinaryOp::Eq || b->op == BinaryOp::Ne || b->op == BinaryOp::Lt || b->op == BinaryOp::Le || b->op == BinaryOp::Gt || b->op == BinaryOp::Ge) {
            std::string i1 = emitComparison(out, b);
            std::string i1z = nextTemp();
            {
                std::string ir = "  "; ir += i1z; ir += " = uitofp i1 "; ir += i1; ir += " to double";
                out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(cmp) -> " << ir; log() << m.str() << CH_LF; }
            }
            return i1z;
        }
        auto L = emitExpr(out, b->lhs.get(), "");
        auto R = emitExpr(out, b->rhs.get(), "");
        std::string res = nextTemp();
        switch (b->op) {
            case BinaryOp::Add: {
                const bool lhsStr = isa<StringExpr>(b->lhs.get()) ||
                    (
                        isa<VarExpr>(b->lhs.get()) &&
                        !dyn_cast<VarExpr>(b->lhs.get())->name.empty() &&
                        dyn_cast<VarExpr>(b->lhs.get())->name.back() == CH_DOLLARSIGN
                    ) ||
                    (
                        isa<BinaryExpr>(b->lhs.get()) &&
                        dyn_cast<BinaryExpr>(b->lhs.get())->op == BinaryOp::Add
                    );
                const bool rhsStr = isa<StringExpr>(b->rhs.get()) ||
                    (
                        isa<VarExpr>(b->rhs.get()) &&
                        !dyn_cast<VarExpr>(b->rhs.get())->name.empty() &&
                        dyn_cast<VarExpr>(b->rhs.get())->name.back() == CH_DOLLARSIGN
                    ) ||
                    (
                        isa<BinaryExpr>(b->rhs.get()) &&
                        dyn_cast<BinaryExpr>(b->rhs.get())->op == BinaryOp::Add
                    );
                if (lhsStr && rhsStr) {
                    // String concatenation: malloc(strlen(L)+strlen(R)+1); strcpy; strcat
                    std::string lenL = nextTemp();
                    {
                        std::string ir = std::format("  {} = call i64 @strlen(ptr {})", lenL, L);
                        out << ir << STR_LF;
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " StrLen L -> " << ir; log() << m.str() << CH_LF;
                        }
                    }
                    std::string lenR = nextTemp();
                    {
                        std::string ir = std::format("  {} = call i64 @strlen(ptr {})", lenR, R);
                        out << ir << STR_LF;
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " StrLen R -> " << ir; log() << m.str() << CH_LF;
                        }
                    }
                    std::string total = nextTemp();
                    {
                        std::string ir = std::format("  {} = add i64 {}, {}", total, lenL, lenR);
                        out << ir << STR_LF;
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " Add lens -> " << ir; log() << m.str() << CH_LF;
                        }
                    }
                    std::string total1 = nextTemp();
                    {
                        std::string ir = std::format("  {} = add i64 {}, 1", total1, total);
                        out << ir << STR_LF;
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " +1 -> " << ir; log() << m.str() << CH_LF;
                        }
                    }
                    std::string buf = nextTemp();
                    {
                        std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, total1);
                        out << ir << STR_LF;
                        {
                            std::ostringstream m; m << "line " << currentLine_ << " malloc -> " << ir; log() << m.str() << CH_LF;
                        }
                    }
                    {
                        std::string ir = std::format("  call ptr @strcpy(ptr {}, ptr {})", buf, L);
                        out << ir << STR_LF;
                        {
                            std::ostringstream m; m << "line " << currentLine_ << " strcpy -> " << ir; log() << m.str() << CH_LF;
                        }
                    }
                    {
                        std::string ir = std::format("  call ptr @strcat(ptr {}, ptr {})", buf, R);
                        out << ir << STR_LF;
                        {
                            std::ostringstream m; m << "line " << currentLine_ << " strcat -> " << ir;
                            log() << m.str() << CH_LF;
                        }
                    }
                    return buf;
                }
        std::string ir = std::format("  {} = fadd double {}, {}", res, L, R); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(+) -> " << ir; log() << m.str() << CH_LF; } break; }
            case BinaryOp::Sub: { std::string ir = std::format("  {} = fsub double {}, {}", res, L, R); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(-) -> " << ir; log() << m.str() << CH_LF; } break; }
            case BinaryOp::Mul: { std::string ir = std::format("  {} = fmul double {}, {}", res, L, R); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(*) -> " << ir; log() << m.str() << CH_LF; } break; }
            case BinaryOp::Div: { std::string ir = std::format("  {} = fdiv double {}, {}", res, L, R); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(/) -> " << ir; log() << m.str() << CH_LF; } break; }
            default: throw CodeGenError("Unsupported binary op in arithmetic");
        }
        return res;
    }
    if (auto call = dyn_cast<const CallExpr>(e)) {
        // Normalize function name to upper for matching
        std::string fn = call->callee; for (auto& ch : fn) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        // Treat known arrays as array element references (1-D)
        if (arraySizes_.contains(call->callee)) {
            const int len = arraySizes_[call->callee];
            ensureArrayAllocated(out, call->callee, len);
            std::string base = arrayAllocaName_[call->callee];
            // Evaluate index (assume numeric), convert to i64
            std::string idxReg = emitExpr(out, call->args[0].get(), "");
            std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Array idx -> " << ir; log() << m.str() << CH_LF; } }
            // gep to element: ptr elem = getelementptr [len x double], ptr base, i64 0, i64 idx
            std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Array elem gep -> " << ir; log() << m.str() << CH_LF; } }
            // load double
            std::string res = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", res, elem); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Array load -> " << ir; log() << m.str() << CH_LF; } }
            return res;
        }
        // Emit args
        std::vector<std::string> argv; argv.reserve(call->args.size());
        for (const auto& a : call->args) argv.push_back(emitExpr(out, a.get(), ""));
        std::string res = nextTemp();
        if (fn == "SQR" || fn == "SQRT") {
            std::string ir = std::format("  {} = call double @sqrt(double {})", res, argv[0]);
            out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sqrt -> " << ir; log() << m.str() << CH_LF; }
            return res;
        }
        if (fn == "ABS") {
            std::string ir = std::format("  {} = call double @fabs(double {})", res, argv[0]);
            out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fabs -> " << ir; log() << m.str() << CH_LF; }
            return res;
        }
        if (fn == "SIN") { std::string ir = std::format("  {} = call double @sin(double {})", res, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sin -> " << ir; log() << m.str() << CH_LF; } return res; }
        if (fn == "COS") { std::string ir = std::format("  {} = call double @cos(double {})", res, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr cos -> " << ir; log() << m.str() << CH_LF; } return res; }
        if (fn == "TAN") { std::string ir = std::format("  {} = call double @tan(double {})", res, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr tan -> " << ir; log() << m.str() << CH_LF; } return res; }
        if (fn == "ATN") { std::string ir = std::format("  {} = call double @atan(double {})", res, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr atan -> " << ir; log() << m.str() << CH_LF; } return res; }
        if (fn == "LOG") { std::string ir = std::format("  {} = call double @log(double {})", res, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr log -> " << ir; log() << m.str() << CH_LF; } return res; }
        if (fn == "EXP") { std::string ir = std::format("  {} = call double @exp(double {})", res, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr exp -> " << ir; log() << m.str() << CH_LF; } return res; }
        if (fn == "INT") { std::string ir = std::format("  {} = call double @floor(double {})", res, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr floor(INT) -> " << ir; log() << m.str() << CH_LF; } return res; }
        if (fn == "FIX") {
            std::string ireg = nextTemp();
            { std::string ir = std::format("  {} = fptosi double {} to i64", ireg, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fix fptosi -> " << ir; log() << m.str() << CH_LF; } }
            { std::string ir = std::format("  {} = sitofp i64 {} to double", res, ireg); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fix sitofp -> " << ir; log() << m.str() << CH_LF; } }
            return res;
        }
        if (fn == "SGN") {
            // sgn(x) = (x>0) - (x<0)
            std::string cmpPos = nextTemp();
            { std::string ir = std::format("  {} = fcmp ogt double {}, 0.0", cmpPos, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn cmp>0 -> " << ir; log() << m.str() << CH_LF; } }
            std::string posD = nextTemp();
            { std::string ir = std::format("  {} = uitofp i1 {} to double", posD, cmpPos); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn pos to dbl -> " << ir; log() << m.str() << CH_LF; } }
            std::string cmpNeg = nextTemp();
            { std::string ir = std::format("  {} = fcmp olt double {}, 0.0", cmpNeg, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn cmp<0 -> " << ir; log() << m.str() << CH_LF; } }
            std::string negD = nextTemp();
            { std::string ir = std::format("  {} = uitofp i1 {} to double", negD, cmpNeg); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn neg to dbl -> " << ir; log() << m.str() << CH_LF; } }
            std::string negVal = nextTemp();
            { std::string ir = std::format("  {} = fsub double 0.0, {}", negVal, negD); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn negate -> " << ir; log() << m.str() << CH_LF; } }
            { std::string ir = std::format("  {} = fadd double {}, {}", res, posD, negVal); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn add -> " << ir; log() << m.str() << CH_LF; } }
            return res;
        }
        if (fn == "RND") { std::string ir = std::format("  {} = call double @gwb_rnd(double {})", res, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr rnd(full) -> " << ir; log() << m.str() << CH_LF; } return res; }
        if (fn == "CINT") {
            std::string ir = std::format("  {} = call double @round(double {})", res, argv[0]);
            out << ir << STR_LF;
            { std::ostringstream m; m << "line " << currentLine_ << " CallExpr cint(round) -> " << ir; log() << m.str() << CH_LF; }
            return res;
        }
        if (fn == "CSNG") {
            // Truncate to single precision and widen back to double
            std::string f = nextTemp();
            { std::string ir = std::format("  {} = fptrunc double {} to float", f, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr csng fptrunc -> " << ir; log() << m.str() << CH_LF; } }
            { std::string ir = std::format("  {} = fpext float {} to double", res, f); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr csng fpext -> " << ir; log() << m.str() << CH_LF; } }
            return res;
        }
        if (fn == "CDBL") {
            // Already double; pass-through
            return argv[0];
        }
        throw CodeGenError("Unknown function call");
    }
    if (auto s = dyn_cast<const StringExpr>(e)) {
        int id = strLiteralId_[s->value];
        std::string gep = nextTemp();
        std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", gep, globalStringName(id));
        out << ir << STR_LF;
        std::ostringstream m; m << "line " << currentLine_ << " StringExpr -> " << ir; log() << m.str() << CH_LF;
        return gep;
    }
    throw CodeGenError("Unknown expression kind");
}

} // namespace gwbasic
