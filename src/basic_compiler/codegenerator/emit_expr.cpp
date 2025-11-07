// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include "basic_compiler/Symbols.h"
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
        // Special-case INKEY$ (treated as built-in string function without parentheses)
        {
            std::string up = v->name; for (auto &ch : up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            if (up == "INKEY$") {
                std::string p = nextTemp();
                { std::string ir = std::format("  {} = getelementptr inbounds [1 x i8], ptr @.str_empty, i64 0, i64 0", p); out << ir << Symbols::LF; }
                log() << "line " << currentLine_ << " VarExpr(INKEY$) -> empty string" << Symbols::LF;
                return p;
            }
        }
        ensureVarAllocated(out, v->name);
        std::string a = varAllocaName_[v->name];
        std::string r = nextTemp();

        if (isStringVarNameCG(v->name)) {
            std::string ld = std::format("  {} = load ptr, ptr {}", r, a);
            out << ld << Symbols::LF;
            // Wrap null pointers to empty string for safety
            std::string safe = nextTemp();
            { std::string ir = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, r); out << ir << Symbols::LF; }
            r = safe;
            log() << "line " << currentLine_ << " VarExpr$ -> load+safe" << Symbols::LF;
        } else {
            // Load typed storage and convert to double for expression math
            switch (numKindOf(v->name)) {
                case NumKind::Int16: {
                    std::string l = nextTemp();
                    { std::string ir = std::format("  {} = load i16, ptr {}", l, a); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  {} = sitofp i16 {} to double", r, l); out << ir << Symbols::LF; }
                    break;
                }
                case NumKind::Long32: {
                    std::string l = nextTemp();
                    { std::string ir = std::format("  {} = load i32, ptr {}", l, a); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  {} = sitofp i32 {} to double", r, l); out << ir << Symbols::LF; }
                    break;
                }
                case NumKind::Single: {
                    std::string l = nextTemp();
                    { std::string ir = std::format("  {} = load float, ptr {}", l, a); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  {} = fpext float {} to double", r, l); out << ir << Symbols::LF; }
                    break;
                }
                case NumKind::Double: {
                    std::string ir = std::format("  {} = load double, ptr {}", r, a);
                    out << ir << Symbols::LF;
                    break;
                }
            }
            log() <<"line " << currentLine_ << " VarExpr -> load/convert to double" << Symbols::LF;
        }
        return r;
    }
    if (auto u = dyn_cast<const UnaryExpr>(e)) {
        auto inner = emitExpr(out, u->inner.get(), "");
        if (u->op == Symbols::PLUS.first()) return inner;
        if (u->op == Symbols::MINUS.first()) {
            std::string res = nextTemp();
            std::string ir = std::format("  {} = fsub double 0.0, {}", res, inner);
            out << ir << Symbols::LF;
            std::ostringstream m; m << "line " << currentLine_ << " UnaryExpr(-) -> " << ir; log() << m.str() << Symbols::LF;
            return res;
        }
        if (u->op == Symbols::EXCLAMATION.first()) {
            // Logical NOT (numeric truthy): 1.0 if inner == 0.0 else 0.0
            std::string isZero = nextTemp();
            { std::string ir = std::format("  {} = fcmp oeq double {}, 0.0", isZero, inner); out << ir << Symbols::LF; }
            std::string res = nextTemp();
            { std::string ir = std::format("  {} = uitofp i1 {} to double", res, isZero); out << ir << Symbols::LF; }
            return res;
        }
    }
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
        if (b->op == BinaryOp::Eq || b->op == BinaryOp::Ne || b->op == BinaryOp::Lt || b->op == BinaryOp::Le || b->op == BinaryOp::Gt || b->op == BinaryOp::Ge) {
            std::string i1 = emitComparison(out, b);
            std::string i1z = nextTemp();
            {
                std::string ir = "  "; ir += i1z; ir += " = uitofp i1 "; ir += i1; ir += " to double";
                out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(cmp) -> " << ir; log() << m.str() << Symbols::LF; }
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
                        out << ir << Symbols::LF;
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " StrLen L -> " << ir; log() << m.str() << Symbols::LF;
                        }
                    }
                    std::string lenR = nextTemp();
                    {
                        std::string ir = std::format("  {} = call i64 @strlen(ptr {})", lenR, R);
                        out << ir << Symbols::LF;
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " StrLen R -> " << ir; log() << m.str() << Symbols::LF;
                        }
                    }
                    std::string total = nextTemp();
                    {
                        std::string ir = std::format("  {} = add i64 {}, {}", total, lenL, lenR);
                        out << ir << Symbols::LF;
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " Add lens -> " << ir; log() << m.str() << Symbols::LF;
                        }
                    }
                    std::string total1 = nextTemp();
                    {
                        std::string ir = std::format("  {} = add i64 {}, 1", total1, total);
                        out << ir << Symbols::LF;
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " +1 -> " << ir; log() << m.str() << Symbols::LF;
                        }
                    }
                    std::string buf = nextTemp();
                    {
                        std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, total1);
                        out << ir << Symbols::LF;
                        {
                            std::ostringstream m; m << "line " << currentLine_ << " malloc -> " << ir; log() << m.str() << Symbols::LF;
                        }
                    }
                    {
                        std::string ir = std::format("  call ptr @strcpy(ptr {}, ptr {})", buf, L);
                        out << ir << Symbols::LF;
                        {
                            std::ostringstream m; m << "line " << currentLine_ << " strcpy -> " << ir; log() << m.str() << Symbols::LF;
                        }
                    }
                    {
                        std::string ir = std::format("  call ptr @strcat(ptr {}, ptr {})", buf, R);
                        out << ir << Symbols::LF;
                        {
                            std::ostringstream m; m << "line " << currentLine_ << " strcat -> " << ir;
                            log() << m.str() << Symbols::LF;
                        }
                    }
                    return buf;
                }
        std::string ir = std::format("  {} = fadd double {}, {}", res, L, R); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(+) -> " << ir; log() << m.str() << Symbols::LF; } break; }
            case BinaryOp::Sub: { std::string ir = std::format("  {} = fsub double {}, {}", res, L, R); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(-) -> " << ir; log() << m.str() << Symbols::LF; } break; }
            case BinaryOp::Mul: { std::string ir = std::format("  {} = fmul double {}, {}", res, L, R); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(*) -> " << ir; log() << m.str() << Symbols::LF; } break; }
            case BinaryOp::Div: {
                // Division with zero-divide error trap
                std::string isz = nextTemp(); { std::string ir = std::format("  {} = fcmp oeq double {}, 0.0", isz, R); out << ir << Symbols::LF; }
                std::string okLbl = lineLabelName(currentLine_) + std::string("_div_ok_") + std::to_string(++tempCounter_);
                std::string errLbl = lineLabelName(currentLine_) + std::string("_div_err_") + std::to_string(tempCounter_);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isz, errLbl, okLbl); out << ir << Symbols::LF; }
                // Error path: set division-by-zero error and dispatch
                out << errLbl << ":" << Symbols::LF;
                { std::string ir = std::format("  store i32 11, ptr @gwb_err_code"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 0, ptr @gwb_resume_stmt"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
                ensureVarAllocated(out, "ERR"); ensureVarAllocated(out, "ERL");
                { std::string derr = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 11 to double", derr); out << ir << Symbols::LF; } storeNumberToVar(out, "ERR", derr); }
                { std::string dln  = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", dln, currentLine_); out << ir << Symbols::LF; } storeNumberToVar(out, "ERL", dln); }
                {
                    std::string trap = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_err_trap_line", trap); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  switch i32 {}, label %exit [", trap); out << ir << Symbols::LF; }
                    for (const auto & [lnum, lp] : lineMap_) { (void)lp; std::string ir = std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)); out << ir << Symbols::LF; }
                    out << "  ]" << Symbols::LF;
                }
                // Ok path computes the division
                out << okLbl << ":" << Symbols::LF;
                { std::string ir = std::format("  {} = fdiv double {}, {}", res, L, R); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(/) -> " << ir; log() << m.str() << Symbols::LF; } }
                break;
            }
            case BinaryOp::IntDiv: {
                // Integer division: truncates toward zero on integerized operands; trap on zero divisor
                std::string li = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", li, L); out << ir << Symbols::LF; }
                std::string ri = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", ri, R); out << ir << Symbols::LF; }
                std::string isz = nextTemp(); { std::string ir = std::format("  {} = icmp eq i64 {}, 0", isz, ri); out << ir << Symbols::LF; }
                std::string okLbl = lineLabelName(currentLine_) + std::string("_idiv_ok_") + std::to_string(++tempCounter_);
                std::string errLbl = lineLabelName(currentLine_) + std::string("_idiv_err_") + std::to_string(tempCounter_);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isz, errLbl, okLbl); out << ir << Symbols::LF; }
                // Error path: division by zero
                out << errLbl << ":" << Symbols::LF;
                { std::string ir = std::format("  store i32 11, ptr @gwb_err_code"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 0, ptr @gwb_resume_stmt"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
                ensureVarAllocated(out, "ERR"); ensureVarAllocated(out, "ERL");
                { std::string derr = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 11 to double", derr); out << ir << Symbols::LF; } storeNumberToVar(out, "ERR", derr); }
                { std::string dln  = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", dln, currentLine_); out << ir << Symbols::LF; } storeNumberToVar(out, "ERL", dln); }
                {
                    std::string trap = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_err_trap_line", trap); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  switch i32 {}, label %exit [", trap); out << ir << Symbols::LF; }
                    for (const auto & [lnum, lp] : lineMap_) { (void)lp; std::string ir = std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)); out << ir << Symbols::LF; }
                    out << "  ]" << Symbols::LF;
                }
                // Ok path computes integer division
                out << okLbl << ":" << Symbols::LF;
                std::string q  = nextTemp(); { std::string ir = std::format("  {} = sdiv i64 {}, {}", q, li, ri); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = sitofp i64 {} to double", res, q); out << ir << Symbols::LF; }
                break;
            }
            case BinaryOp::Mod: {
                // Integer remainder with sign of dividend; trap on zero divisor
                std::string li = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", li, L); out << ir << Symbols::LF; }
                std::string ri = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", ri, R); out << ir << Symbols::LF; }
                std::string isz = nextTemp(); { std::string ir = std::format("  {} = icmp eq i64 {}, 0", isz, ri); out << ir << Symbols::LF; }
                std::string okLbl = lineLabelName(currentLine_) + std::string("_mod_ok_") + std::to_string(++tempCounter_);
                std::string errLbl = lineLabelName(currentLine_) + std::string("_mod_err_") + std::to_string(tempCounter_);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isz, errLbl, okLbl); out << ir << Symbols::LF; }
                // Error path: division by zero
                out << errLbl << ":" << Symbols::LF;
                { std::string ir = std::format("  store i32 11, ptr @gwb_err_code"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 0, ptr @gwb_resume_stmt"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
                ensureVarAllocated(out, "ERR"); ensureVarAllocated(out, "ERL");
                { std::string derr = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 11 to double", derr); out << ir << Symbols::LF; } storeNumberToVar(out, "ERR", derr); }
                { std::string dln  = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", dln, currentLine_); out << ir << Symbols::LF; } storeNumberToVar(out, "ERL", dln); }
                {
                    std::string trap = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_err_trap_line", trap); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  switch i32 {}, label %exit [", trap); out << ir << Symbols::LF; }
                    for (const auto & [lnum, lp] : lineMap_) { (void)lp; std::string ir = std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)); out << ir << Symbols::LF; }
                    out << "  ]" << Symbols::LF;
                }
                // Ok path computes remainder
                out << okLbl << ":" << Symbols::LF;
                std::string rmd= nextTemp(); { std::string ir = std::format("  {} = srem i64 {}, {}", rmd, li, ri); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = sitofp i64 {} to double", res, rmd); out << ir << Symbols::LF; }
                break;
            }
            case BinaryOp::And: {
                std::string lb = nextTemp(); { std::string ir = std::format("  {} = fcmp one double {}, 0.0", lb, L); out << ir << Symbols::LF; }
                std::string rb = nextTemp(); { std::string ir = std::format("  {} = fcmp one double {}, 0.0", rb, R); out << ir << Symbols::LF; }
                std::string bb = nextTemp(); { std::string ir = std::format("  {} = and i1 {}, {}", bb, lb, rb); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = uitofp i1 {} to double", res, bb); out << ir << Symbols::LF; }
                break;
            }
            case BinaryOp::Or: {
                std::string lb = nextTemp(); { std::string ir = std::format("  {} = fcmp one double {}, 0.0", lb, L); out << ir << Symbols::LF; }
                std::string rb = nextTemp(); { std::string ir = std::format("  {} = fcmp one double {}, 0.0", rb, R); out << ir << Symbols::LF; }
                std::string bb = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bb, lb, rb); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = uitofp i1 {} to double", res, bb); out << ir << Symbols::LF; }
                break;
            }
            case BinaryOp::Pow: {
                std::string ir = std::format("  {} = call double @pow(double {}, double {})", res, L, R);
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " BinaryExpr(^) -> " << ir; log() << m.str() << Symbols::LF; }
                break;
            }
            default: throw CodeGenError("Unsupported binary op in arithmetic");
        }
        return res;
    }
    if (auto call = dyn_cast<const CallExpr>(e)) {
        // Array element reference A(i[,j...]) handled as call-form
        if (arrayDims_.contains(call->callee)) {
            const auto &dims = arrayDims_[call->callee];
            long long total = 1; for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); if (ext < 0) ext = 0; total *= ext; }
            // Convert indices and bounds-check
            std::vector<std::string> idxI64s; idxI64s.reserve(call->args.size());
            std::vector<std::string> bads; bads.reserve(call->args.size());
            for (size_t di = 0; di < call->args.size(); ++di) {
                std::string idxD = emitExpr(out, call->args[di].get(), "");
                std::string idxI = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI, idxD); out << ir << Symbols::LF; }
                idxI64s.push_back(idxI);
                std::string ltBase = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI, optionBase_); out << ir << Symbols::LF; }
                std::string gtUb = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI, dims[di]); out << ir << Symbols::LF; }
                std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb); out << ir << Symbols::LF; }
                bads.push_back(bad);
            }
            std::string anyBad = bads.empty() ? std::string("false") : bads[0];
            for (size_t i = 1; i < bads.size(); ++i) { std::string nb = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]); out << ir << Symbols::LF; } anyBad = nb; }
            std::string okLbl = lineLabelName(currentLine_) + std::string("_arr_ref_ok_") + std::to_string(++tempCounter_);
            std::string errLbl = lineLabelName(currentLine_) + std::string("_arr_ref_err_") + std::to_string(tempCounter_);
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, okLbl); out << ir << Symbols::LF; }
            // Error path: set error and dispatch
            out << errLbl << ":" << Symbols::LF;
            { std::string ir = std::format("  store i32 9, ptr @gwb_err_code"); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 0, ptr @gwb_resume_stmt"); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
            // Mirror into ERR/ERL variables for runtime bounds errors
            ensureVarAllocated(out, "ERR");
            ensureVarAllocated(out, "ERL");
            { std::string derr = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 9 to double", derr); out << ir << Symbols::LF; } storeNumberToVar(out, "ERR", derr); }
            { std::string dln = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", dln, currentLine_); out << ir << Symbols::LF; } storeNumberToVar(out, "ERL", dln); }
            {
                std::string trap = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_err_trap_line", trap); out << ir << Symbols::LF; }
                { std::string ir = std::format("  switch i32 {}, label %exit [", trap); out << ir << Symbols::LF; }
                for (const auto & [lnum, lp] : lineMap_) { (void)lp; std::string ir = std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)); out << ir << Symbols::LF; }
                out << "  ]" << Symbols::LF;
            }
            // Ok path
            out << okLbl << ":" << Symbols::LF;
            std::vector<long long> extents; extents.reserve(dims.size());
            for (size_t di = 0; di < dims.size(); ++di) { long long e = static_cast<long long>(dims[di]) - optionBase_ + 1; if (e < 0) e = 0; extents.push_back(e); }
            std::vector<long long> strides(dims.size(), 1);
            for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) { strides[di] = strides[di + 1] * extents[di + 1]; }
            std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
            for (const auto& ii : idxI64s) { std::string a = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", a, ii, optionBase_); out << ir << Symbols::LF; } adjs.push_back(a); }
            std::string lin = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]); out << ir << Symbols::LF; }
            for (size_t di = 1; di < adjs.size(); ++di) { std::string t = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", t, adjs[di], strides[di]); out << ir << Symbols::LF; } std::string s = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", s, lin, t); out << ir << Symbols::LF; } lin = s; }
            if (isStringVarNameCG(call->callee)) {
                ensureStringArrayAllocated(out, call->callee, static_cast<int>(total));
                std::string baseArr = arrayAllocaName_[call->callee];
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, baseArr, lin); out << ir << Symbols::LF; }
                std::string val = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", val, elem); out << ir << Symbols::LF; }
                std::string safe = nextTemp(); { std::string ir = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, val); out << ir << Symbols::LF; }
                return safe;
            } else {
                ensureArrayAllocated(out, call->callee, static_cast<int>(total));
                std::string baseArr = arrayAllocaName_[call->callee];
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, total, arrayElemType(call->callee), baseArr, lin); out << ir << Symbols::LF; }
                return loadArrayElemAsDouble(out, call->callee, elem);
            }
        }
        // Normalize function name to upper for matching
        std::string fn = call->callee; for (auto& ch : fn) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        // Emit args
        std::vector<std::string> argv; argv.reserve(call->args.size());
        for (const auto& a : call->args) argv.push_back(emitExpr(out, a.get(), ""));
        std::string res = nextTemp();
        if (fn == "SQR" || (allowSqrtAlias_ && fn == "SQRT")) {
            std::string ir = std::format("  {} = call double @sqrt(double {})", res, argv[0]);
            out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sqrt -> " << ir; log() << m.str() << Symbols::LF; }
            return res;
        }
        if (fn == "ABS") {
            std::string ir = std::format("  {} = call double @fabs(double {})", res, argv[0]);
            out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fabs -> " << ir; log() << m.str() << Symbols::LF; }
            return res;
        }
        if (fn == "SIN") { std::string ir = std::format("  {} = call double @sin(double {})", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sin -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "COS") { std::string ir = std::format("  {} = call double @cos(double {})", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr cos -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "TAN") { std::string ir = std::format("  {} = call double @tan(double {})", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr tan -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "ATN") { std::string ir = std::format("  {} = call double @atan(double {})", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr atan -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "LOG") { std::string ir = std::format("  {} = call double @log(double {})", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr log -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "VAL") { std::string ir = std::format("  {} = call double @strtod(ptr {}, ptr null)", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr val -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "LEN") {
            // LEN: string expression -> strlen; numeric variable/array element -> storage size (bytes)
            const Expr* arg0 = call->args[0].get();
            // If argument is variable
            if (auto v = dyn_cast<const VarExpr>(arg0)) {
                if (!isStringVarNameCG(v->name)) {
                    int bytes = 4;
                    switch (numKindOf(v->name)) {
                        case NumKind::Int16: bytes = 2; break;
                        case NumKind::Long32: bytes = 4; break;
                        case NumKind::Single: bytes = 4; break;
                        case NumKind::Double: bytes = 8; break;
                    }
                    char buf[16]; std::snprintf(buf, sizeof(buf), "%d.0", bytes);
                    return std::string(buf);
                }
            }
            // If argument is array element reference
            if (auto ac = dyn_cast<const CallExpr>(arg0)) {
                if (arrayDims_.contains(ac->callee) && !isStringVarNameCG(ac->callee)) {
                    int bytes = 4;
                    switch (numKindOf(ac->callee)) {
                        case NumKind::Int16: bytes = 2; break;
                        case NumKind::Long32: bytes = 4; break;
                        case NumKind::Single: bytes = 4; break;
                        case NumKind::Double: bytes = 8; break;
                    }
                    char buf[16]; std::snprintf(buf, sizeof(buf), "%d.0", bytes);
                    return std::string(buf);
                }
            }
            // Otherwise, treat as string and call strlen
            std::string n = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", n, argv[0]); out << ir << Symbols::LF; }
            { std::string ir = std::format("  {} = sitofp i64 {} to double", res, n); out << ir << Symbols::LF; }
            { std::ostringstream m; m << "line " << currentLine_ << " CallExpr len -> len->double"; log() << m.str() << Symbols::LF; }
            return res;
        }
        if (fn == "INSTR") {
            // INSTR(s$, sub$) or INSTR(start, s$, sub$)
            std::string s = argv.size() == 2 ? argv[0] : argv[1];
            std::string sub = argv.size() == 2 ? argv[1] : argv[2];
            std::string starti = "";
            if (argv.size() == 3) {
                starti = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", starti, argv[0]); out << ir << Symbols::LF; }
            } else {
                starti = nextTemp(); { std::string ir = std::format("  {} = add i64 1, 0", starti); out << ir << Symbols::LF; }
            }
            // off = max(0, start-1)
            std::string off0 = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", off0, starti); out << ir << Symbols::LF; }
            std::string isNeg = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", isNeg, off0); out << ir << Symbols::LF; }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 0, i64 {}", off, isNeg, off0); out << ir << Symbols::LF; }
            // Clamp to length: off = min(off, strlen(s))
            std::string slen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", slen, s); out << ir << Symbols::LF; }
            std::string offGt = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, {}", offGt, off, slen); out << ir << Symbols::LF; }
            std::string offClamped = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", offClamped, offGt, slen, off); out << ir << Symbols::LF; }
            // sOff = s + off; p = strstr(sOff, sub)
            std::string sOff = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", sOff, s, offClamped); out << ir << Symbols::LF; }
            std::string p = nextTemp(); { std::string ir = std::format("  {} = call ptr @strstr(ptr {}, ptr {})", p, sOff, sub); out << ir << Symbols::LF; }
            std::string p_i = nextTemp(); { std::string ir = std::format("  {} = ptrtoint ptr {} to i64", p_i, p); out << ir << Symbols::LF; }
            std::string s_i = nextTemp(); { std::string ir = std::format("  {} = ptrtoint ptr {} to i64", s_i, s); out << ir << Symbols::LF; }
            std::string diff = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", diff, p_i, s_i); out << ir << Symbols::LF; }
            std::string idx1 = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", idx1, diff); out << ir << Symbols::LF; }
            std::string idxd = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", idxd, idx1); out << ir << Symbols::LF; }
            std::string isNull = nextTemp(); { std::string ir = std::format("  {} = icmp eq ptr {}, null", isNull, p); out << ir << Symbols::LF; }
            std::string sel = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, double 0.0, double {}", sel, isNull, idxd); out << ir << Symbols::LF; }
            return sel;
        }
        if (fn == "EXP") { std::string ir = std::format("  {} = call double @exp(double {})", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr exp -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "INT") { std::string ir = std::format("  {} = call double @floor(double {})", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr floor(INT) -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "FIX") {
            std::string ireg = nextTemp();
            { std::string ir = std::format("  {} = fptosi double {} to i64", ireg, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fix fptosi -> " << ir; log() << m.str() << Symbols::LF; } }
            { std::string ir = std::format("  {} = sitofp i64 {} to double", res, ireg); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr fix sitofp -> " << ir; log() << m.str() << Symbols::LF; } }
            return res;
        }
        if (fn == "SGN") {
            // sgn(x) = (x>0) - (x<0)
            std::string cmpPos = nextTemp();
            { std::string ir = std::format("  {} = fcmp ogt double {}, 0.0", cmpPos, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn cmp>0 -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string posD = nextTemp();
            { std::string ir = std::format("  {} = uitofp i1 {} to double", posD, cmpPos); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn pos to dbl -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string cmpNeg = nextTemp();
            { std::string ir = std::format("  {} = fcmp olt double {}, 0.0", cmpNeg, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn cmp<0 -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string negD = nextTemp();
            { std::string ir = std::format("  {} = uitofp i1 {} to double", negD, cmpNeg); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn neg to dbl -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string negVal = nextTemp();
            { std::string ir = std::format("  {} = fsub double 0.0, {}", negVal, negD); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn negate -> " << ir; log() << m.str() << Symbols::LF; } }
            { std::string ir = std::format("  {} = fadd double {}, {}", res, posD, negVal); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr sgn add -> " << ir; log() << m.str() << Symbols::LF; } }
            return res;
        }
        if (fn == "RND") { std::string ir = std::format("  {} = call double @gwb_rnd(double {})", res, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr rnd(full) -> " << ir; log() << m.str() << Symbols::LF; } return res; }
        if (fn == "ASC") {
            // argv[0] is ptr to string; strict check: empty -> error 5
            std::string slen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", slen, argv[0]); out << ir << Symbols::LF; }
            std::string isEmpty = nextTemp(); { std::string ir = std::format("  {} = icmp eq i64 {}, 0", isEmpty, slen); out << ir << Symbols::LF; }
            std::string okLbl = lineLabelName(currentLine_) + std::string("_asc_ok_") + std::to_string(++tempCounter_);
            std::string errLbl = lineLabelName(currentLine_) + std::string("_asc_err_") + std::to_string(tempCounter_);
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isEmpty, errLbl, okLbl); out << ir << Symbols::LF; }
            // Error path: Illegal function call (5)
            out << errLbl << ":" << Symbols::LF;
            { std::string ir = std::format("  store i32 5, ptr @gwb_err_code"); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 0, ptr @gwb_resume_stmt"); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
            ensureVarAllocated(out, "ERR"); ensureVarAllocated(out, "ERL");
            { std::string derr = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 5 to double", derr); out << ir << Symbols::LF; } storeNumberToVar(out, "ERR", derr); }
            { std::string dln  = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", dln, currentLine_); out << ir << Symbols::LF; } storeNumberToVar(out, "ERL", dln); }
            {
                std::string trap = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_err_trap_line", trap); out << ir << Symbols::LF; }
                { std::string ir = std::format("  switch i32 {}, label %exit [", trap); out << ir << Symbols::LF; }
                for (const auto & [lnum, lp] : lineMap_) { (void)lp; std::string ir = std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)); out << ir << Symbols::LF; }
                out << "  ]" << Symbols::LF;
            }
            // Ok path: load first byte and return as double
            out << okLbl << ":" << Symbols::LF;
            std::string b = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", b, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr asc load -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string i32v = nextTemp(); { std::string ir = std::format("  {} = zext i8 {} to i32", i32v, b); out << ir << Symbols::LF; }
            { std::string ir = std::format("  {} = uitofp i32 {} to double", res, i32v); out << ir << Symbols::LF; }
            return res;
        }
        if (fn == "SCREEN") {
            // SCREEN(row, col [, z]) -> ASCII code at 1-based (row,col)
            // Convert to 0-based, clamp to bounds, load from @gwb_screen
            std::string r64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", r64, argv[0]); out << ir << Symbols::LF; }
            std::string c64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", c64, argv[1]); out << ir << Symbols::LF; }
            std::string r0 = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", r0, r64); out << ir << Symbols::LF; }
            std::string c0 = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", c0, c64); out << ir << Symbols::LF; }
            // Clamp row in [0,24]
            std::string rlo = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", rlo, r0); out << ir << Symbols::LF; }
            std::string rsel0 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 0, i64 {}", rsel0, rlo, r0); out << ir << Symbols::LF; }
            std::string rhi = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, 24", rhi, rsel0); out << ir << Symbols::LF; }
            std::string rsel = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 24, i64 {}", rsel, rhi, rsel0); out << ir << Symbols::LF; }
            // Clamp col in [0,79]
            std::string clo = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", clo, c0); out << ir << Symbols::LF; }
            std::string csel0 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 0, i64 {}", csel0, clo, c0); out << ir << Symbols::LF; }
            std::string chi = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, 79", chi, csel0); out << ir << Symbols::LF; }
            std::string csel = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 79, i64 {}", csel, chi, csel0); out << ir << Symbols::LF; }
            std::string r32 = nextTemp(); { std::string ir = std::format("  {} = trunc i64 {} to i32", r32, rsel); out << ir << Symbols::LF; }
            std::string c32 = nextTemp(); { std::string ir = std::format("  {} = trunc i64 {} to i32", c32, csel); out << ir << Symbols::LF; }
            std::string r80 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 80", r80, r32); out << ir << Symbols::LF; }
            std::string idx32 = nextTemp(); { std::string ir = std::format("  {} = add i32 {}, {}", idx32, r80, c32); out << ir << Symbols::LF; }
            std::string idx64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", idx64, idx32); out << ir << Symbols::LF; }
            std::string p = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [2000 x i8], ptr @gwb_screen, i64 0, i64 {}", p, idx64); out << ir << Symbols::LF; }
            std::string b = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", b, p); out << ir << Symbols::LF; }
            std::string i32v = nextTemp(); { std::string ir = std::format("  {} = zext i8 {} to i32", i32v, b); out << ir << Symbols::LF; }
            { std::string ir = std::format("  {} = uitofp i32 {} to double", res, i32v); out << ir << Symbols::LF; }
            return res;
        }
        if (fn == "PEEK") {
            // addr = seg*16 + arg
            std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK load seg -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK seg*16 -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK off fptosi -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK seg sext -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK addr -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string base = "%tmem"; base = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", base, addr); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK gep -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string b = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", b, base); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PEEK load -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string i32v = nextTemp(); { std::string ir = std::format("  {} = zext i8 {} to i32", i32v, b); out << ir << Symbols::LF; }
            { std::string ir = std::format("  {} = uitofp i32 {} to double", res, i32v); out << ir << Symbols::LF; }
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
            out << ir << Symbols::LF;
            { std::ostringstream m; m << "line " << currentLine_ << " CallExpr cint(round) -> " << ir; log() << m.str() << Symbols::LF; }
            return res;
        }
        if (fn == "CSNG") {
            // Truncate to single precision and widen back to double
            std::string f = nextTemp();
            { std::string ir = std::format("  {} = fptrunc double {} to float", f, argv[0]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr csng fptrunc -> " << ir; log() << m.str() << Symbols::LF; } }
            { std::string ir = std::format("  {} = fpext float {} to double", res, f); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr csng fpext -> " << ir; log() << m.str() << Symbols::LF; } }
            return res;
        }
        if (fn == "CDBL") {
            // Already double; pass-through
            return argv[0];
        }
        if (fn == "LEFT$") {
            // LEFT$(s$, n)
            std::string n64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n64, argv[1]); out << ir << Symbols::LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, n64); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", buf, argv[0], n64); out << ir << Symbols::LF; }
            std::string pN = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN, buf, n64); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN); out << ir << Symbols::LF; }
            return buf;
        }
        if (fn == "RIGHT$") {
            // RIGHT$(s$, n)
            std::string n64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n64, argv[1]); out << ir << Symbols::LF; }
            std::string len = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", len, argv[0]); out << ir << Symbols::LF; }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", off, len, n64); out << ir << Symbols::LF; }
            std::string src = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", src, argv[0], off); out << ir << Symbols::LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, n64); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", buf, src, n64); out << ir << Symbols::LF; }
            std::string pN = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN, buf, n64); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN); out << ir << Symbols::LF; }
            return buf;
        }
        if (fn == "MID$") {
            // MID$(s$, start [, len]) with 1-based index
            std::string starti = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", starti, argv[1]); out << ir << Symbols::LF; }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", off, starti); out << ir << Symbols::LF; }
            std::string src = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", src, argv[0], off); out << ir << Symbols::LF; }
            std::string n64;
            if (call->args.size() >= 3) {
                n64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n64, argv[2]); out << ir << Symbols::LF; }
            } else {
                n64 = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", n64, src); out << ir << Symbols::LF; }
            }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, n64); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", buf, src, n64); out << ir << Symbols::LF; }
            std::string pN = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN, buf, n64); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN); out << ir << Symbols::LF; }
            return buf;
        }
        if (fn == "STR$") {
            // Convert numeric to string via snprintf into sbuf, then copy to fresh buffer
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [3 x i8], ptr @.fmt_num_ns, i64 0, i64 0", fmt); out << ir << Symbols::LF; }
            std::string sbuf = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})", sbuf, fmt, argv[0]); out << ir << Symbols::LF; }
            std::string slen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", slen, sbuf); out << ir << Symbols::LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, slen); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strcpy(ptr {}, ptr {})", buf, sbuf); out << ir << Symbols::LF; }
            return buf;
        }
        if (fn == "SPACE$") {
            // SPACE$(n): repeat space character n times
            std::string n64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n64, argv[0]); out << ir << Symbols::LF; }
            // clamp negative to 0
            std::string isNeg = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", isNeg, n64); out << ir << Symbols::LF; }
            std::string nsel = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 0, i64 {}", nsel, isNeg, n64); out << ir << Symbols::LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, nsel); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @memset(ptr {}, i32 32, i64 {})", buf, nsel); out << ir << Symbols::LF; }
            std::string pN = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN, buf, nsel); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN); out << ir << Symbols::LF; }
            return buf;
        }
        if (fn == "STRING$") {
            // STRING$(n, x): n copies of first byte of string x or numeric x
            std::string n64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n64, argv[0]); out << ir << Symbols::LF; }
            std::string isNeg = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", isNeg, n64); out << ir << Symbols::LF; }
            std::string nsel = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 0, i64 {}", nsel, isNeg, n64); out << ir << Symbols::LF; }
            // Determine fill byte
            std::string fillb;
            if (isStringExpr(call->args[1].get())) {
                std::string b = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", b, argv[1]); out << ir << Symbols::LF; }
                fillb = b;
            } else {
                std::string i32v = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", i32v, argv[1]); out << ir << Symbols::LF; }
                std::string b = nextTemp(); { std::string ir = std::format("  {} = trunc i32 {} to i8", b, i32v); out << ir << Symbols::LF; }
                fillb = b;
            }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, nsel); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << Symbols::LF; }
            // memset requires i32 value; zext i8 to i32
            std::string val32 = nextTemp(); { std::string ir = std::format("  {} = zext i8 {} to i32", val32, fillb); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @memset(ptr {}, i32 {}, i64 {})", buf, val32, nsel); out << ir << Symbols::LF; }
            std::string pN = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN, buf, nsel); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN); out << ir << Symbols::LF; }
            return buf;
        }
        if (fn == "LTRIM$") {
            // LTRIM$(s$): skip leading spaces
            std::string src = argv[0];
            std::string slen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", slen, src); out << ir << Symbols::LF; }
            // Loop i from 0 while i < slen and src[i] == ' '
            std::string loopLbl = lineLabelName(currentLine_) + std::string("_ltr_loop_") + std::to_string(++tempCounter_);
            std::string bodyLbl = lineLabelName(currentLine_) + std::string("_ltr_body_") + std::to_string(tempCounter_);
            std::string contLbl = lineLabelName(currentLine_) + std::string("_ltr_cont_") + std::to_string(tempCounter_);
            std::string doneLbl = lineLabelName(currentLine_) + std::string("_ltr_done_") + std::to_string(tempCounter_);
            out << loopLbl << ":" << Symbols::LF;
            std::string i = nextTemp(); { std::string ir = std::format("  {} = phi i64 [ 0, %{} ], [ %{}, %{} ]", i, lineLabelName(currentLine_), contLbl, contLbl); out << ir << Symbols::LF; }
            std::string cond = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", cond, i, slen); out << ir << Symbols::LF; }
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", cond, bodyLbl, doneLbl); out << ir << Symbols::LF; }
            out << bodyLbl << ":" << Symbols::LF;
            std::string p = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", p, src, i); out << ir << Symbols::LF; }
            std::string ch = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", ch, p); out << ir << Symbols::LF; }
            std::string isSp = nextTemp(); { std::string ir = std::format("  {} = icmp eq i8 {}, 32", isSp, ch); out << ir << Symbols::LF; }
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isSp, contLbl, doneLbl); out << ir << Symbols::LF; }
            out << contLbl << ":" << Symbols::LF;
            std::string inc = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", inc, i); out << ir << Symbols::LF; }
            { std::string ir = std::format("  br label %{}", loopLbl); out << ir << Symbols::LF; }
            out << doneLbl << ":" << Symbols::LF;
            // i is index of first non-space; compute remainder length and copy
            std::string start = i;
            std::string rem = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", rem, slen, start); out << ir << Symbols::LF; }
            std::string sptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", sptr, src, start); out << ir << Symbols::LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, rem); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", buf, sptr, rem); out << ir << Symbols::LF; }
            std::string pN2 = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN2, buf, rem); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN2); out << ir << Symbols::LF; }
            return buf;
        }
        if (fn == "RTRIM$") {
            // RTRIM$(s$): trim trailing spaces
            std::string src = argv[0];
            std::string slen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", slen, src); out << ir << Symbols::LF; }
            std::string loopLbl = lineLabelName(currentLine_) + std::string("_rtr_loop_") + std::to_string(++tempCounter_);
            std::string bodyLbl = lineLabelName(currentLine_) + std::string("_rtr_body_") + std::to_string(tempCounter_);
            std::string contLbl = lineLabelName(currentLine_) + std::string("_rtr_cont_") + std::to_string(tempCounter_);
            std::string doneLbl = lineLabelName(currentLine_) + std::string("_rtr_done_") + std::to_string(tempCounter_);
            out << loopLbl << ":" << Symbols::LF;
            std::string j = nextTemp(); { std::string ir = std::format("  {} = phi i64 [ {}, %{} ], [ %{}, %{} ]", j, slen, lineLabelName(currentLine_), contLbl, contLbl); out << ir << Symbols::LF; }
            std::string cnd = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, 0", cnd, j); out << ir << Symbols::LF; }
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", cnd, bodyLbl, doneLbl); out << ir << Symbols::LF; }
            out << bodyLbl << ":" << Symbols::LF;
            std::string jm1 = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", jm1, j); out << ir << Symbols::LF; }
            std::string p = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", p, src, jm1); out << ir << Symbols::LF; }
            std::string ch = nextTemp(); { std::string ir = std::format("  {} = load i8, ptr {}", ch, p); out << ir << Symbols::LF; }
            std::string isSp = nextTemp(); { std::string ir = std::format("  {} = icmp eq i8 {}, 32", isSp, ch); out << ir << Symbols::LF; }
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isSp, contLbl, doneLbl); out << ir << Symbols::LF; }
            out << contLbl << ":" << Symbols::LF;
            { std::string ir = std::format("  br label %{}", loopLbl); out << ir << Symbols::LF; }
            out << doneLbl << ":" << Symbols::LF;
            std::string m = j; // number of bytes to keep
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, m); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", buf, src, m); out << ir << Symbols::LF; }
            std::string pN2 = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pN2, buf, m); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", pN2); out << ir << Symbols::LF; }
            return buf;
        }
        if (fn == "CHR$") {
            // Allocate 2 bytes and store low 8 bits of numeric arg as char
            // Strict: if arg outside 0..255 -> error 5
            std::string lt0 = nextTemp(); { std::string ir = std::format("  {} = fcmp olt double {}, 0.0", lt0, argv[0]); out << ir << Symbols::LF; }
            std::string gt255 = nextTemp(); { std::string ir = std::format("  {} = fcmp ogt double {}, 255.0", gt255, argv[0]); out << ir << Symbols::LF; }
            std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, lt0, gt255); out << ir << Symbols::LF; }
            std::string okLbl = lineLabelName(currentLine_) + std::string("_chr_ok_") + std::to_string(++tempCounter_);
            std::string errLbl = lineLabelName(currentLine_) + std::string("_chr_err_") + std::to_string(tempCounter_);
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", bad, errLbl, okLbl); out << ir << Symbols::LF; }
            out << errLbl << ":" << Symbols::LF;
            { std::string ir = std::format("  store i32 5, ptr @gwb_err_code"); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 0, ptr @gwb_resume_stmt"); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
            ensureVarAllocated(out, "ERR"); ensureVarAllocated(out, "ERL");
            { std::string derr = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 5 to double", derr); out << ir << Symbols::LF; } storeNumberToVar(out, "ERR", derr); }
            { std::string dln  = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", dln, currentLine_); out << ir << Symbols::LF; } storeNumberToVar(out, "ERL", dln); }
            {
                std::string trap = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_err_trap_line", trap); out << ir << Symbols::LF; }
                { std::string ir = std::format("  switch i32 {}, label %exit [", trap); out << ir << Symbols::LF; }
                for (const auto & [lnum, lp] : lineMap_) { (void)lp; std::string ir = std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)); out << ir << Symbols::LF; }
                out << "  ]" << Symbols::LF;
            }
            // Ok path
            out << okLbl << ":" << Symbols::LF;
            std::string two = nextTemp(); { std::string ir = std::format("  {} = add i64 1, 1", two); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 2)", buf); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CallExpr chr$ malloc -> " << ir; log() << m.str() << Symbols::LF; } }
            std::string ival = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", ival, argv[0]); out << ir << Symbols::LF; }
            std::string b = nextTemp(); { std::string ir = std::format("  {} = trunc i32 {} to i8", b, ival); out << ir << Symbols::LF; }
            std::string p0 = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", p0, buf); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 {}, ptr {}", b, p0); out << ir << Symbols::LF; }
            std::string p1 = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 1", p1, buf); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i8 0, ptr {}", p1); out << ir << Symbols::LF; }
            return buf;
        }
        throw CodeGenError("Unknown function call");
    }
    if (auto s = dyn_cast<const StringExpr>(e)) {
        int id = strLiteralId_[s->value];
        std::string gep = nextTemp();
        std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", gep, globalStringName(id));
        out << ir << Symbols::LF;
        std::ostringstream m; m << "line " << currentLine_ << " StringExpr -> " << ir; log() << m.str() << Symbols::LF;
        return gep;
    }
    throw CodeGenError("Unknown expression kind");
}

} // namespace gwbasic
