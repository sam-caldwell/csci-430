// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/ColorStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include <sstream>
#include <format>
#include <cmath>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitWhile
 * Inputs:
 *  - out: IR output stream to append to
 *  - ws: Parsed WhileStmt node
 *  - currLineLabel: Base label for naming emitted blocks
 *  - localCounter: Per-line counter to uniquify labels
 * Outputs:
 *  - void (appends IR for WHILE loop structure)
 * Theory of operation:
 *  - Emits a loop with condition, body, and back-edge, merging at an end
 *    label. The condition must be a comparison expression.
 */
void CodeGenerator::emitWhile(std::ostringstream& out, const WhileStmt* ws, const std::string& currLineLabel, int& localCounter) {
    std::string id = std::to_string(++localCounter);
    std::string condLbl = currLineLabel; condLbl += "_while_cond"; condLbl += id;
    std::string bodyLbl = currLineLabel; bodyLbl += "_while_body"; bodyLbl += id;
    std::string endLbl  = currLineLabel; endLbl  += "_while_end";  endLbl  += id;

    // Jump to condition
    { std::string ir = std::format("  br label %{}", condLbl); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While -> " << ir << Symbols::LF; }

    // Condition
    out << condLbl << ":" << Symbols::LF;
    std::string cond;
    if (auto be = dyn_cast<const BinaryExpr>(ws->cond.get());
        be && (be->op == BinaryOp::Eq || be->op == BinaryOp::Ne || be->op == BinaryOp::Lt || be->op == BinaryOp::Le || be->op == BinaryOp::Gt || be->op == BinaryOp::Ge)) {
        cond = emitComparison(out, be);
    } else {
        std::string val = emitExpr(out, ws->cond.get(), "");
        cond = nextTemp();
        { std::string ir = std::format("  {} = fcmp one double {}, 0.0", cond, val); out << ir << Symbols::LF; }
    }
    { std::string ir = std::format("  br i1 {}, label %{}, label %{}", cond, bodyLbl, endLbl); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While branch -> " << ir << Symbols::LF; }

    // Body
    out << bodyLbl << ":" << Symbols::LF;
    for (const auto& s : ws->body) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
                std::string ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body Assign -> " << ir << Symbols::LF;
            } else {
                storeNumberToVar(out, asg->name, val);
            }
        } else if (auto mid = dyn_cast<MidAssignStmt>(s.get())) {
            std::string dest;
            std::string storePtr;
            if (!mid->indices.empty()) {
                const auto &dims = arrayDims_[mid->name];
                long long total = 1; for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); if (ext < 0) ext = 0; total *= ext; }
                ensureStringArrayAllocated(out, mid->name, static_cast<int>(total));
                std::string base = arrayAllocaName_[mid->name];
                std::vector<std::string> idxI64s; idxI64s.reserve(mid->indices.size());
                std::vector<std::string> bads; bads.reserve(mid->indices.size());
                for (size_t di = 0; di < mid->indices.size(); ++di) {
                    std::string idxReg = emitExpr(out, mid->indices[di].get(), currLineLabel);
                    std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; }
                    idxI64s.push_back(idxI64);
                    std::string ltBase = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI64, optionBase_); out << ir << Symbols::LF; }
                    std::string gtUb = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI64, dims[di]); out << ir << Symbols::LF; }
                    std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb); out << ir << Symbols::LF; }
                    bads.push_back(bad);
                }
                std::string anyBad = bads[0];
                for (size_t i = 1; i < bads.size(); ++i) { std::string nb = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]); out << ir << Symbols::LF; } anyBad = nb; }
                std::string doLbl = currLineLabel + std::string("_while_mid_ok_") + std::to_string(++localCounter);
                std::string errLbl = currLineLabel + std::string("_while_mid_err_") + std::to_string(localCounter);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl); out << ir << Symbols::LF; }
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
                out << doLbl << ":" << Symbols::LF;
                std::vector<long long> extents; extents.reserve(dims.size());
                for (size_t di = 0; di < dims.size(); ++di) { long long e = static_cast<long long>(dims[di]) - optionBase_ + 1; if (e < 0) e = 0; extents.push_back(e); }
                std::vector<long long> strides(dims.size(), 1);
                for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) { strides[di] = strides[di + 1] * extents[di + 1]; }
                std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
                for (const auto& ii : idxI64s) { std::string a = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", a, ii, optionBase_); out << ir << Symbols::LF; } adjs.push_back(a); }
                std::string lin = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]); out << ir << Symbols::LF; }
                for (size_t di = 1; di < adjs.size(); ++di) { std::string t = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", t, adjs[di], strides[di]); out << ir << Symbols::LF; } std::string s2 = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", s2, lin, t); out << ir << Symbols::LF; } lin = s2; }
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin); out << ir << Symbols::LF; }
                storePtr = elem;
                dest = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", dest, elem); out << ir << Symbols::LF; }
            } else {
                ensureVarAllocated(out, mid->name);
                dest = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", dest, varAllocaName_[mid->name]); out << ir << Symbols::LF; }
                storePtr = varAllocaName_[mid->name];
            }
            std::string startD = emitExpr(out, mid->start.get(), currLineLabel);
            std::string startI = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", startI, startD); out << ir << Symbols::LF; }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", off, startI); out << ir << Symbols::LF; }
            std::string dlen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", dlen, dest); out << ir << Symbols::LF; }
            std::string dsize = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", dsize, dlen); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, dsize); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strcpy(ptr {}, ptr {})", buf, dest); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store ptr {}, ptr {}", buf, storePtr); out << ir << Symbols::LF; }
            dest = buf;
            std::string src = emitExpr(out, mid->value.get(), currLineLabel);
            std::string slen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", slen, src); out << ir << Symbols::LF; }
            std::string n = slen;
            if (mid->len) {
                std::string lenD = emitExpr(out, mid->len.get(), currLineLabel);
                n = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n, lenD); out << ir << Symbols::LF; }
            }
            std::string negOff = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", negOff, off); out << ir << Symbols::LF; }
            std::string geLen = nextTemp(); { std::string ir = std::format("  {} = icmp sge i64 {}, {}", geLen, off, dlen); out << ir << Symbols::LF; }
            std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, negOff, geLen); out << ir << Symbols::LF; }
            std::string doLbl = currLineLabel + std::string("_mid_do_") + std::to_string(++localCounter);
            std::string endLbl2 = currLineLabel + std::string("_mid_end_") + std::to_string(localCounter);
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", bad, endLbl2, doLbl); out << ir << Symbols::LF; }
            out << doLbl << ":" << Symbols::LF;
            std::string avail = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", avail, dlen, off); out << ir << Symbols::LF; }
            std::string n_lt_av = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", n_lt_av, n, avail); out << ir << Symbols::LF; }
            std::string m1 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", m1, n_lt_av, n, avail); out << ir << Symbols::LF; }
            std::string m1_lt_s = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", m1_lt_s, m1, slen); out << ir << Symbols::LF; }
            std::string m2 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", m2, m1_lt_s, m1, slen); out << ir << Symbols::LF; }
            std::string dst = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", dst, dest, off); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", dst, src, m2); out << ir << Symbols::LF; }
            { std::string ir = std::format("  br label %{}", endLbl2); out << ir << Symbols::LF; }
            out << endLbl2 << ":" << Symbols::LF;
        } else if (auto pr = dyn_cast<PrintStmt>(s.get())) {
            std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
            for (size_t pi = 0; pi < items.size(); ++pi) {
                const bool last = (pi + 1 == items.size());
                const Expr* v = items[pi];
                auto isStr = [&](const Expr* e, const auto& self) -> bool {
                    if (isa<StringExpr>(e)) return true;
                    if (auto vv = dyn_cast<VarExpr>(e)) return !vv->name.empty() && vv->name.back() == Symbols::DOLLARSIGN.first();
                    if (auto bb = dyn_cast<BinaryExpr>(e)) return (bb->op == BinaryOp::Add) && (self(bb->lhs.get(), self) || self(bb->rhs.get(), self));
                    return false;
                };
                if (isStr(v, isStr)) {
                    auto sptr = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp(); { std::string ir2 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Print -> " << ir2 << Symbols::LF; }
                    { std::string ir3 = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, sptr); out << ir3 << Symbols::LF; log() << "line " << currentLine_ << " While body Print -> " << ir3 << Symbols::LF; }
                } else {
                    // Constant number? Avoid runtime fcmp in while prints as well
                    if (const auto cnum = dyn_cast<const NumberExpr>(v)) {
                        const double cv = cnum->value;
                        const bool isIntegral = (std::floor(cv) == cv);
                        if (isIntegral) {
                            std::string fmtI = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, (last ? "@.fmt_int" : "@.fmt_int_sp")); out << ir1 << Symbols::LF; }
                            long long iv = static_cast<long long>(cv);
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, iv); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Print int -> " << ir2 << Symbols::LF; }
                        } else {
                            std::string fmtF = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, (last ? "@.fmt_num" : "@.fmt_num_sp")); out << ir1 << Symbols::LF; }
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {:.6f})", fmtF, cv); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Print flt -> " << ir2 << Symbols::LF; }
                        }
                    } else {
                        auto val = emitExpr(out, v, currLineLabel);
                        // Build float and int format pointers
                        std::string fmtF = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, (last ? "@.fmt_num" : "@.fmt_num_sp")); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " While body Print -> " << ir1 << Symbols::LF; }
                        std::string fmtI = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, (last ? "@.fmt_int" : "@.fmt_int_sp")); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " While body Print -> " << ir1 << Symbols::LF; }
                        // Integer detection
                        std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", iv, val); out << ir << Symbols::LF; }
                        std::string dv = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", dv, iv); out << ir << Symbols::LF; }
                        std::string isInt = nextTemp(); { std::string ir = std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val); out << ir << Symbols::LF; }
                        std::string intLbl = currLineLabel + std::string("_wprint_int_") + std::to_string(++localCounter);
                        std::string fltLbl = currLineLabel + std::string("_wprint_flt_") + std::to_string(localCounter);
                        std::string contLbl = currLineLabel + std::string("_wprint_cont_") + std::to_string(localCounter);
                        { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl); out << ir << Symbols::LF; }
                        out << intLbl << ":" << Symbols::LF;
                        { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, iv); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Print int -> " << ir2 << Symbols::LF; }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        out << fltLbl << ":" << Symbols::LF;
                        { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmtF, val); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Print flt -> " << ir2 << Symbols::LF; }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        out << contLbl << ":" << Symbols::LF;
                    }
                }
            }
        } else if (auto aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
            const auto &dims = arrayDims_[aaset->name];
            long long total = 1; for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); if (ext < 0) ext = 0; total *= ext; }
            std::vector<std::string> idxI64s; idxI64s.reserve(aaset->indices.size());
            std::vector<std::string> bads; bads.reserve(aaset->indices.size());
            for (size_t di = 0; di < aaset->indices.size(); ++di) {
                std::string idxReg = emitExpr(out, aaset->indices[di].get(), currLineLabel);
                std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; }
                idxI64s.push_back(idxI64);
                std::string ltBase = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI64, optionBase_); out << ir << Symbols::LF; }
                std::string gtUb = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI64, dims[di]); out << ir << Symbols::LF; }
                std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb); out << ir << Symbols::LF; }
                bads.push_back(bad);
            }
            std::string anyBad = bads[0];
            for (size_t i = 1; i < bads.size(); ++i) { std::string nb = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]); out << ir << Symbols::LF; } anyBad = nb; }
            std::string doLbl = currLineLabel + std::string("_while_arr_ok_") + std::to_string(++localCounter);
            std::string errLbl = currLineLabel + std::string("_while_arr_err_") + std::to_string(localCounter);
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl); out << ir << Symbols::LF; }
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
            out << doLbl << ":" << Symbols::LF;
            std::vector<long long> extents; extents.reserve(dims.size());
            for (size_t di = 0; di < dims.size(); ++di) { long long e = static_cast<long long>(dims[di]) - optionBase_ + 1; if (e < 0) e = 0; extents.push_back(e); }
            std::vector<long long> strides(dims.size(), 1);
            for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) { strides[di] = strides[di + 1] * extents[di + 1]; }
            std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
            for (const auto& ii : idxI64s) { std::string a = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", a, ii, optionBase_); out << ir << Symbols::LF; } adjs.push_back(a); }
            std::string lin = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]); out << ir << Symbols::LF; }
            for (size_t di = 1; di < adjs.size(); ++di) { std::string t = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", t, adjs[di], strides[di]); out << ir << Symbols::LF; } std::string s2 = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", s2, lin, t); out << ir << Symbols::LF; } lin = s2; }
            if (isStringArrayNameCG(aaset->name)) {
                ensureStringArrayAllocated(out, aaset->name, static_cast<int>(total));
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin); out << ir << Symbols::LF; }
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                { std::string ir = std::format("  store ptr {}, ptr {}", val, elem); out << ir << Symbols::LF; }
            } else {
                ensureArrayAllocated(out, aaset->name, static_cast<int>(total));
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, total, arrayElemType(aaset->name), base, lin); out << ir << Symbols::LF; }
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                storeNumberToArrayElem(out, aaset->name, elem, val);
            }
        } else if (auto fs = dyn_cast<ForStmt>(s.get())) {
            emitFor(out, fs, currLineLabel, localCounter);
        } else if (auto ib = dyn_cast<IfBlockStmt>(s.get())) {
            emitIfBlock(out, ib, currLineLabel, localCounter);
        } else if (auto ws2 = dyn_cast<WhileStmt>(s.get())) {
            emitWhile(out, ws2, currLineLabel, localCounter);
        } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body Return -> " << ir << Symbols::LF;
        } else if (isa<EndStmt>(s.get())) {
            std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body End -> " << ir << Symbols::LF;
        } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = std::format("  br label %{}", lineLabelName(gt->targetLine)); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body Goto -> " << ir << Symbols::LF;
        } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
            std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
            std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
            out << std::format("  br label %{}", entryLbl) << Symbols::LF;
            emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
            out << contLbl << ":" << Symbols::LF;
        } else if (auto og = dyn_cast<OnGotoStmt>(s.get())) {
            std::string idx = emitExpr(out, og->index.get(), currLineLabel);
            std::string idxi32 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", idxi32, idx); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body OnGoto fptosi -> " << ir << Symbols::LF; }
            std::string contLbl = currLineLabel + std::string("_on_cont_") + std::to_string(++localCounter);
            {
                std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                for (size_t i = 0; i < og->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << lineLabelName(og->targets[i]);
                ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " While body OnGoto switch -> " << ir.str() << Symbols::LF;
            }
            out << contLbl << ":" << Symbols::LF;
        } else if (auto ogs = dyn_cast<OnGosubStmt>(s.get())) {
            std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
            std::string idxi32 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", idxi32, idx); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body OnGosub fptosi -> " << ir << Symbols::LF; }
            std::string contLbl = currLineLabel + std::string("_on_gs_cont_") + std::to_string(++localCounter);
            std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
            for (size_t i = 0; i < ogs->targets.size(); ++i) entryLbls.push_back(currLineLabel + std::string("_on_gs_entry_") + std::to_string(localCounter) + std::string("_") + std::to_string(i+1));
            {
                std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                for (size_t i = 0; i < ogs->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << entryLbls[i];
                ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " While body OnGosub switch -> " << ir.str() << Symbols::LF;
            }
            for (size_t i = 0; i < ogs->targets.size(); ++i) emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
            out << contLbl << ":" << Symbols::LF;
        } else if (isa<StopStmt>(s.get())) {
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_); out << ir << Symbols::LF; }
            out << std::format("  br label %{}", endLbl) << Symbols::LF;
        } else if (isa<SystemStmt>(s.get())) {
            out << std::format("  br label %{}", endLbl) << Symbols::LF;
        } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
            ensureVarAllocated(out, ins->name);
            std::string fmt = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " While body Input -> " << ir1 << Symbols::LF; }
            // Read into temp double then convert to var storage
            std::string tmp = nextTemp(); { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
            { std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Input -> " << ir2 << Symbols::LF; }
            std::string dv = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
            storeNumberToVar(out, ins->name, dv);
        } else if (auto col = dyn_cast<ColorStmt>(s.get())) {
            auto emitColor = [&](const std::unique_ptr<Expr>& e, bool isFg){
                if (!e) return;
                std::string val = emitExpr(out, e.get(), "");
                std::string i32v = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", i32v, val); out << ir << Symbols::LF; }
                std::string masked = nextTemp(); { std::string ir = std::format("  {} = and i32 {}, 15", masked, i32v); out << ir << Symbols::LF; }
                std::string idx64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", idx64, masked); out << ir << Symbols::LF; }
                std::string p = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x i32], ptr {}, i64 0, i64 {}", p, (isFg?"@.sgr_fg_tbl":"@.sgr_bg_tbl"), idx64); out << ir << Symbols::LF; }
                std::string code = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr {}", code, p); out << ir << Symbols::LF; }
                std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [7 x i8], ptr @.fmt_sgr, i64 0, i64 0", fmt); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 27, i32 {})", fmt, code); out << ir << Symbols::LF; }
            };
            emitColor(col->fg, true);
            emitColor(col->bg, false);
        } else {
            throw CodeGenError("Unsupported statement in WHILE body");
        }
    }
    // Jump back to condition
    { std::string ir = std::format("  br label %{}", condLbl); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While -> " << ir << Symbols::LF; }
    // End label
    out << endLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
