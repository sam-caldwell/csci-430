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
        // Inline binding for DEF FN parameter?
        std::string bound;
        if (lookupBinding(v->name, bound)) return bound;
        ensureVarAllocated(out, v->name);
        std::string a = varAllocaName_[v->name];
        std::string r = nextTemp();

        if (isStringVarNameCG(v->name)) {
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
                const bool lhsStr = isStringExpr(b->lhs.get());
                const bool rhsStr = isStringExpr(b->rhs.get());
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
        if (fn == "ASC") {
            // argv[0] is ptr to string; load first byte and return as double
            std::string b = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", b, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr asc load -> " << ir; log() << m.str() << CH_LF; } }
            std::string i32v = nextTemp(); { std::string ir = std::format("  {} = zext i8 {} to i32", i32v, b); out << ir << STR_LF; }
            { std::string ir = std::format("  {} = uitofp i32 {} to double", res, i32v); out << ir << STR_LF; }
            return res;
        }
        if (fn == "SCREEN") {
            // SCREEN(row, col [, z]) -> ASCII code at 1-based (row,col)
            // Convert to 0-based, clamp to bounds, load from @gwb_screen
            std::string r64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", r64, argv[0]); out << ir << STR_LF; }
            std::string c64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", c64, argv[1]); out << ir << STR_LF; }
            std::string r0 = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", r0, r64); out << ir << STR_LF; }
            std::string c0 = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", c0, c64); out << ir << STR_LF; }
            // Clamp row in [0,24]
            std::string rlo = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", rlo, r0); out << ir << STR_LF; }
            std::string rsel0 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 0, i64 {}", rsel0, rlo, r0); out << ir << STR_LF; }
            std::string rhi = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, 24", rhi, rsel0); out << ir << STR_LF; }
            std::string rsel = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 24, i64 {}", rsel, rhi, rsel0); out << ir << STR_LF; }
            // Clamp col in [0,79]
            std::string clo = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", clo, c0); out << ir << STR_LF; }
            std::string csel0 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 0, i64 {}", csel0, clo, c0); out << ir << STR_LF; }
            std::string chi = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, 79", chi, csel0); out << ir << STR_LF; }
            std::string csel = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 79, i64 {}", csel, chi, csel0); out << ir << STR_LF; }
            std::string r32 = nextTemp(); { std::string ir = std::format("  {} = trunc i64 {} to i32", r32, rsel); out << ir << STR_LF; }
            std::string c32 = nextTemp(); { std::string ir = std::format("  {} = trunc i64 {} to i32", c32, csel); out << ir << STR_LF; }
            std::string r80 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 80", r80, r32); out << ir << STR_LF; }
            std::string idx32 = nextTemp(); { std::string ir = std::format("  {} = add i32 {}, {}", idx32, r80, c32); out << ir << STR_LF; }
            std::string idx64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", idx64, idx32); out << ir << STR_LF; }
            std::string p = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [2000 x i8], ptr @gwb_screen, i64 0, i64 {}", p, idx64); out << ir << STR_LF; }
            std::string b = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", b, p); out << ir << STR_LF; }
            std::string i32v = nextTemp(); { std::string ir = std::format("  {} = zext i8 {} to i32", i32v, b); out << ir << STR_LF; }
            { std::string ir = std::format("  {} = uitofp i32 {} to double", res, i32v); out << ir << STR_LF; }
            return res;
        }
        if (fn == "PEEK") {
            // addr = seg*16 + arg
            std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK load seg -> " << ir; log() << m.str() << CH_LF; } }
            std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK seg*16 -> " << ir; log() << m.str() << CH_LF; } }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off, argv[0]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK off fptosi -> " << ir; log() << m.str() << CH_LF; } }
            std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK seg sext -> " << ir; log() << m.str() << CH_LF; } }
            std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK addr -> " << ir; log() << m.str() << CH_LF; } }
            std::string base = "%tmem"; base = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", base, addr); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK gep -> " << ir; log() << m.str() << CH_LF; } }
            std::string b = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", b, base); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK load -> " << ir; log() << m.str() << CH_LF; } }
            std::string i32v = nextTemp(); { std::string ir = std::format("  {} = zext i8 {} to i32", i32v, b); out << ir << STR_LF; }
            { std::string ir = std::format("  {} = uitofp i32 {} to double", res, i32v); out << ir << STR_LF; }
            return res;
        }
        if (fn == "USR") {
            // Identity: return argument as-is
            return argv[0];
        }
        // User-defined DEF FN inline expansion
        auto itUF = userFunctions_.find(fn);
        if (itUF != userFunctions_.end()) {
            const DefFnStmt* def = itUF->second;
            // Bind parameter name to evaluated argument
            std::map<std::string, std::string> bmap; bmap[def->paramName] = argv.empty() ? std::string() : argv[0];
            bindingStack_.push_back(std::move(bmap));
            std::string val = emitExpr(out, def->body.get(), "");
            bindingStack_.pop_back();
            return val;
        }
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
        if (fn == "LEFT$") {
            // LEFT$(s$, n)
            std::string n64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n64, argv[1]); out << ir << STR_LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, n64); out << ir << STR_LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << STR_LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", buf, argv[0], n64); out << ir << STR_LF; }
            std::string pN = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN, buf, n64); out << ir << STR_LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN); out << ir << STR_LF; }
            return buf;
        }
        if (fn == "RIGHT$") {
            // RIGHT$(s$, n)
            std::string n64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n64, argv[1]); out << ir << STR_LF; }
            std::string len = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", len, argv[0]); out << ir << STR_LF; }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", off, len, n64); out << ir << STR_LF; }
            std::string src = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", src, argv[0], off); out << ir << STR_LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, n64); out << ir << STR_LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << STR_LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", buf, src, n64); out << ir << STR_LF; }
            std::string pN = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN, buf, n64); out << ir << STR_LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN); out << ir << STR_LF; }
            return buf;
        }
        if (fn == "MID$") {
            // MID$(s$, start [, len]) with 1-based index
            std::string starti = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", starti, argv[1]); out << ir << STR_LF; }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", off, starti); out << ir << STR_LF; }
            std::string src = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", src, argv[0], off); out << ir << STR_LF; }
            std::string n64;
            if (call->args.size() >= 3) {
                n64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n64, argv[2]); out << ir << STR_LF; }
            } else {
                n64 = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", n64, src); out << ir << STR_LF; }
            }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, n64); out << ir << STR_LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << STR_LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", buf, src, n64); out << ir << STR_LF; }
            std::string pN = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN, buf, n64); out << ir << STR_LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN); out << ir << STR_LF; }
            return buf;
        }
        if (fn == "CHR$") {
            // Allocate 2 bytes and store low 8 bits of numeric arg as char
            std::string two = nextTemp(); { std::string ir = std::format("  {} = add i64 1, 1", two); out << ir << STR_LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 2)", buf); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr chr$ malloc -> " << ir; log() << m.str() << CH_LF; } }
            std::string ival = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", ival, argv[0]); out << ir << STR_LF; }
            std::string b = nextTemp(); { std::string ir = std::format("  {} = trunc i32 {} to i8", b, ival); out << ir << STR_LF; }
            std::string p0 = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", p0, buf); out << ir << STR_LF; }
            { std::string ir = std::format("  store i8 {}, ptr {}", b, p0); out << ir << STR_LF; }
            std::string p1 = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 1", p1, buf); out << ir << STR_LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", p1); out << ir << STR_LF; }
            return buf;
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
