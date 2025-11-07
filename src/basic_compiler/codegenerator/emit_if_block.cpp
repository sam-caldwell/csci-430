// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include <sstream>
#include <format>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitIfBlock
 * Inputs:
 *  - out: IR output stream to append to
 *  - ib: Parsed IfBlockStmt node (with optional ELSE body)
 *  - currLineLabel: Base label for naming emitted blocks
 *  - localCounter: Per-line counter to uniquify labels
 * Outputs:
 *  - void (appends IR for IF/THEN[/ELSE] block)
 * Theory of operation:
 *  - Lowers the condition as a comparison, emits branches to THEN/ELSE,
 *    lowers contained statements, and merges control at an end label.
 */
void CodeGenerator::emitIfBlock(std::ostringstream& out, const IfBlockStmt* ib, const std::string& currLineLabel, int& localCounter) {
    std::string ifId = std::to_string(++localCounter);
    std::string thenLbl = currLineLabel; thenLbl += "_if_then"; thenLbl += ifId;
    std::string elseLbl = currLineLabel; elseLbl += "_if_else"; elseLbl += ifId;
    std::string endLbl  = currLineLabel; endLbl  += "_if_end";  endLbl  += ifId;

    // Emit condition: accept either a comparison or a general truthy numeric expression
    std::string cond;
    if (auto be = dyn_cast<const BinaryExpr>(ib->cond.get());
        be && (be->op == BinaryOp::Eq || be->op == BinaryOp::Ne || be->op == BinaryOp::Lt || be->op == BinaryOp::Le || be->op == BinaryOp::Gt || be->op == BinaryOp::Ge)) {
        cond = emitComparison(out, be);
    } else {
        // General boolean: treat non-zero as true
        std::string val = emitExpr(out, ib->cond.get(), "");
        cond = nextTemp();
        { std::string ir = std::format("  {} = fcmp one double {}, 0.0", cond, val); out << ir << Symbols::LF; }
    }
    if (!ib->elseBody.empty()) {
        std::string br = "  br i1 "; br += cond; br += ", label %"; br += thenLbl; br += ", label %"; br += elseLbl;
        out << br << Symbols::LF; log() << "line " << currentLine_ << " IfBlock -> " << br << Symbols::LF;
    } else {
        std::string br = "  br i1 "; br += cond; br += ", label %"; br += thenLbl; br += ", label %"; br += endLbl;
        out << br << Symbols::LF; log() << "line " << currentLine_ << " IfBlock -> " << br << Symbols::LF;
    }

    // THEN body
    out << thenLbl << ":" << Symbols::LF;
    bool thenTerminated = false;
    for (const auto& s : ib->thenBody) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
                std::string ir = "  "; ir += "store ptr "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Assign$ -> " << ir << Symbols::LF;
            } else {
                // Numeric assignment honors semantic numeric kind per variable
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
                std::string doLbl = currLineLabel + std::string("_mid_ok2_") + std::to_string(++localCounter);
                std::string errLbl = currLineLabel + std::string("_mid_err2_") + std::to_string(localCounter);
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
            // Full PRINT support: strings/numbers, USING override, and optional channel
            std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
            auto emit_pad_to_next_zone = [&]() {
                if (!printZones_) return;
                std::string col = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_cur_col", col); out << ir << Symbols::LF; }
                std::string mod = nextTemp(); { std::string ir = std::format("  {} = srem i32 {}, 14", mod, col); out << ir << Symbols::LF; }
                std::string isZero = nextTemp(); { std::string ir = std::format("  {} = icmp eq i32 {}, 0", isZero, mod); out << ir << Symbols::LF; }
                std::string sub = nextTemp(); { std::string ir = std::format("  {} = sub i32 14, {}", sub, mod); out << ir << Symbols::LF; }
                std::string pad = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i32 14, i32 {}", pad, isZero, sub); out << ir << Symbols::LF; }
                std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_pad, i64 0", fmt); out << ir << Symbols::LF; }
                std::string spaces = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [15 x i8], ptr @.spaces_14, i64 0, i64 0", spaces); out << ir << Symbols::LF; }
                if (pr->channel >= 1) {
                    std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                    std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i32 {}, ptr {})", fh, fmt, pad, spaces); out << ir << Symbols::LF; }
                } else {
                    { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {}, ptr {})", fmt, pad, spaces); out << ir << Symbols::LF; }
                    std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                    std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i32 {}, ptr {})", n, sbuf, fmt, pad, spaces); out << irn << Symbols::LF; }
                    std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                    { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                }
            };
            for (size_t pi = 0; pi < items.size(); ++pi) {
                const bool last = (pi + 1 == items.size());
                const bool addNL = last && (pr->trail == PrintStmt::Terminator::Newline);
                const Expr* v = items[pi];
                if (isStringExpr(v)) {
                    auto sptr = emitExpr(out, v, currLineLabel);
                    std::string defFmt = nextTemp(); { const char* sym = addNL ? "@.fmt_str" : "@.fmt_str_sp"; std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", defFmt, sym); out << ir << Symbols::LF; }
                    std::string useFmt = defFmt;
                    if (pr->format) useFmt = emitExpr(out, pr->format.get(), currLineLabel);
                    if (pr->channel >= 1) {
                        std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                        std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                        { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fh, useFmt, sptr); out << ir << Symbols::LF; }
                    } else {
                        { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", useFmt, sptr); out << ir << Symbols::LF; }
                        std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                        std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, useFmt, sptr); out << irn << Symbols::LF; }
                        std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                        { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                    }
                } else {
                    bool nextStartsWithSpace = false;
                    if (!last && (pi + 1) < items.size()) {
                        if (const auto* ns = dyn_cast<const StringExpr>(items[pi + 1])) { if (!ns->value.empty() && ns->value.front() == ' ') nextStartsWithSpace = true; }
                    }
                    auto val = emitExpr(out, v, currLineLabel);
                    std::string fmtF = nextTemp(); { const char* sym = addNL ? "@.fmt_num" : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp"); std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, sym); out << ir << Symbols::LF; }
                    std::string fmtI = nextTemp(); { const char* symI = addNL ? "@.fmt_int" : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp"); std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, symI); out << ir << Symbols::LF; }
                    if (pr->format) {
                        std::string useFmt = emitExpr(out, pr->format.get(), currLineLabel);
                        std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", iv, val); out << ir << Symbols::LF; }
                        std::string dv = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", dv, iv); out << ir << Symbols::LF; }
                        std::string isInt = nextTemp(); { std::string ir = std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val); out << ir << Symbols::LF; }
                        std::string intLbl = currLineLabel + std::string("_print_int_") + std::to_string(++localCounter);
                        std::string fltLbl = currLineLabel + std::string("_print_flt_") + std::to_string(localCounter);
                        std::string contLbl = currLineLabel + std::string("_print_cont_") + std::to_string(localCounter);
                        { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl); out << ir << Symbols::LF; }
                        out << intLbl << ":" << Symbols::LF;
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                            { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fh, useFmt, iv); out << ir << Symbols::LF; }
                        } else {
                            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", useFmt, iv); out << ir << Symbols::LF; }
                        }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        out << fltLbl << ":" << Symbols::LF;
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                            { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, useFmt, val); out << ir << Symbols::LF; }
                        } else {
                            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", useFmt, val); out << ir << Symbols::LF; }
                        }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        out << contLbl << ":" << Symbols::LF;
                    } else {
                        std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", iv, val); out << ir << Symbols::LF; }
                        std::string dv = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", dv, iv); out << ir << Symbols::LF; }
                        std::string isInt = nextTemp(); { std::string ir = std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val); out << ir << Symbols::LF; }
                        std::string intLbl = currLineLabel + std::string("_print_int_") + std::to_string(++localCounter);
                        std::string fltLbl = currLineLabel + std::string("_print_flt_") + std::to_string(localCounter);
                        std::string contLbl = currLineLabel + std::string("_print_cont_") + std::to_string(localCounter);
                        { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl); out << ir << Symbols::LF; }
                        // Integer path
                        out << intLbl << ":" << Symbols::LF;
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                            { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fh, fmtI, iv); out << ir << Symbols::LF; }
                        } else {
                            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, iv); out << ir << Symbols::LF; }
                            std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                            std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i64 {})", n, sbuf, fmtI, iv); out << irn << Symbols::LF; }
                            std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                            { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                        }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        // Float path
                        out << fltLbl << ":" << Symbols::LF;
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                            { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, fmtF, val); out << ir << Symbols::LF; }
                        } else {
                            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmtF, val); out << ir << Symbols::LF; }
                            std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                            std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})", n, sbuf, fmtF, val); out << irn << Symbols::LF; }
                            std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                            { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                        }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        out << contLbl << ":" << Symbols::LF;
                    }
                }
                // Separator behavior: pad to next 14-col zone on a comma
                if (!last) {
                    if (pi < pr->seps.size() && pr->seps[pi] == PrintStmt::Sep::Comma) {
                        emit_pad_to_next_zone();
                    }
                }
            }
            // Trailing terminator for PRINT in IF block
            if (items.empty()) {
                if (pr->trail == PrintStmt::Terminator::Newline) {
                    std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_str, i64 0", fmt); out << ir << Symbols::LF; }
                    std::string empty = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.str_empty, i64 0", empty); out << ir << Symbols::LF; }
                    if (pr->channel >= 1) {
                        std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                        std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                        { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fh, fmt, empty); out << ir << Symbols::LF; }
                    } else {
                        { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, empty); out << ir << Symbols::LF; }
                        std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                        std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, fmt, empty); out << irn << Symbols::LF; }
                        std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                        { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                    }
                } else if (pr->trail == PrintStmt::Terminator::Comma) {
                    // Trailing comma: pad to next zone
                    emit_pad_to_next_zone();
                }
            } else if (!items.empty() && pr->trail == PrintStmt::Terminator::Comma) {
                // Items present and trailing comma: pad to next zone
                emit_pad_to_next_zone();
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
            std::string doLbl = currLineLabel + std::string("_arr_ok_") + std::to_string(++localCounter);
            std::string errLbl = currLineLabel + std::string("_arr_err_") + std::to_string(localCounter);
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
        } else if (auto ws = dyn_cast<WhileStmt>(s.get())) {
            emitWhile(out, ws, currLineLabel, localCounter);
        } else if (auto ib2 = dyn_cast<IfBlockStmt>(s.get())) {
            emitIfBlock(out, ib2, currLineLabel, localCounter);
        } else if (auto is = dyn_cast<IfStmt>(s.get())) {
            auto be2 = dyn_cast<BinaryExpr>(is->cond.get());
            if (!be2 || (be2->op != BinaryOp::Eq && be2->op != BinaryOp::Ne && be2->op != BinaryOp::Lt && be2->op != BinaryOp::Le && be2->op != BinaryOp::Gt && be2->op != BinaryOp::Ge)) {
                throw CodeGenError("IF condition must be a comparison");
            }
            std::string cond2 = emitComparison(out, be2);
            std::string contLbl2 = currLineLabel + std::string("_cont_") + std::to_string(++localCounter);
            std::string irb2 = std::format("  br i1 {}, label %{}, label %{}", cond2, lineLabelName(is->targetLine), contLbl2);
            out << irb2 << Symbols::LF; log() << "line " << currentLine_ << " IfStmt -> " << irb2 << Symbols::LF;
            out << contLbl2 << ":" << Symbols::LF;
        } else if (auto rz = dyn_cast<RandomizeStmt>(s.get())) {
            if (rz->seed) {
                auto val = emitExpr(out, rz->seed.get(), currLineLabel);
                std::string si = nextTemp(); { std::string ir = "  "; ir += si; ir += " = fptosi double "; ir += val; ir += " to i64"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Randomize fptosi -> " << ir << Symbols::LF; }
                { std::string ir = "  call void @srand48(i64 "; ir += si; ir += ")"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Randomize srand48 -> " << ir << Symbols::LF; }
            } else {
                std::string t = nextTemp(); { std::string ir = "  "; ir += t; ir += " = call i64 @time(ptr null)"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Randomize time -> " << ir << Symbols::LF; }
                { std::string ir = "  call void @srand48(i64 "; ir += t; ir += ")"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Randomize srand48(time) -> " << ir << Symbols::LF; }
            }
            } else if (auto og = dyn_cast<OnGotoStmt>(s.get())) {
                std::string idx = emitExpr(out, og->index.get(), currLineLabel);
                std::string idxi32 = nextTemp(); { std::string ir = "  "; ir += idxi32; ir += " = fptosi double "; ir += idx; ir += " to i32"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then OnGoto fptosi -> " << ir << Symbols::LF; }
                std::string contLbl = currLineLabel + std::string("_on_cont_") + std::to_string(++localCounter);
                {
                    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < og->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << lineLabelName(og->targets[i]);
                    ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then OnGoto switch -> " << ir.str() << Symbols::LF;
                }
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ogs = dyn_cast<OnGosubStmt>(s.get())) {
                std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
                std::string idxi32 = nextTemp(); { std::string ir = "  "; ir += idxi32; ir += " = fptosi double "; ir += idx; ir += " to i32"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then OnGosub fptosi -> " << ir << Symbols::LF; }
                std::string contLbl = currLineLabel + std::string("_on_gs_cont_") + std::to_string(++localCounter);
                std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
                for (size_t i = 0; i < ogs->targets.size(); ++i) entryLbls.push_back(currLineLabel + std::string("_on_gs_entry_") + std::to_string(localCounter) + std::string("_") + std::to_string(i+1));
                {
                    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < ogs->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << entryLbls[i];
                    ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then OnGosub switch -> " << ir.str() << Symbols::LF;
                }
                for (size_t i = 0; i < ogs->targets.size(); ++i) emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
                out << contLbl << ":" << Symbols::LF;
            } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Return -> " << ir << Symbols::LF; thenTerminated = true; break;
            } else if (isa<EndStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then End -> " << ir << Symbols::LF; thenTerminated = true; break;
            } else if (isa<StopStmt>(s.get())) {
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_); out << ir << Symbols::LF; }
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Stop -> break+exit" << Symbols::LF; thenTerminated = true; break;
            } else if (isa<SystemStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then System -> " << ir << Symbols::LF; thenTerminated = true; break;
            } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine); out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Goto -> " << ir << Symbols::LF;
            } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
                std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
                std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
                out << "  br label %" << entryLbl << Symbols::LF;
                emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
            // Optional prompt
            if (ins->promptLiteral || ins->promptVar) {
                std::string pstr;
                if (ins->promptLiteral) {
                    if (strLiteralId_.contains(*ins->promptLiteral)) {
                        int id = strLiteralId_[*ins->promptLiteral];
                        pstr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", pstr, globalStringName(id)); out << ir << Symbols::LF; }
                    }
                } else {
                    ensureVarAllocated(out, *ins->promptVar);
                    pstr = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", pstr, varAllocaName_[*ins->promptVar]); out << ir << Symbols::LF; }
                    std::string safe = nextTemp(); { std::string ir = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, pstr); out << ir << Symbols::LF; } pstr = safe;
                }
                if (!pstr.empty()) {
                    std::string fmtS = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_str_sp, i64 0", fmtS); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmtS, pstr); out << ir << Symbols::LF; }
                }
            }
            for (const auto& vname : ins->variables) {
                ensureVarAllocated(out, vname);
                std::string fmt = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; }
                std::string tmp = nextTemp(); { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
                { std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp); out << ir2 << Symbols::LF; }
                std::string dv = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
                storeNumberToVar(out, vname, dv);
            }
        } else {
            throw CodeGenError(std::string("Unsupported statement in IF body (THEN): ")
                + nodeName(s.get()));
        }
    }
    // Branch to end when THEN body completes
    if (!thenTerminated) out << "  br label %" << endLbl << Symbols::LF;

    // ELSE body, if present
    if (!ib->elseBody.empty()) {
    out << elseLbl << ":" << Symbols::LF;
        bool elseTerminated = false;
        for (const auto& s : ib->elseBody) {
            if (auto asg = dyn_cast<AssignStmt>(s.get())) {
                std::string val = emitExpr(out, asg->value.get(), currLineLabel);
                if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
                    std::string ir = "  "; ir += "store ptr "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
                    out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Assign$ -> " << ir << Symbols::LF;
                } else {
                    // Numeric assignment honors semantic numeric kind per variable
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
                std::string doLbl = currLineLabel + std::string("_mid_ok3_") + std::to_string(++localCounter);
                std::string errLbl = currLineLabel + std::string("_mid_err3_") + std::to_string(localCounter);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl); out << ir << Symbols::LF; }
                out << errLbl << ":" << Symbols::LF;
                { std::string ir = std::format("  store i32 9, ptr @gwb_err_code"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 0, ptr @gwb_resume_stmt"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
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
                // Full PRINT support in ELSE: strings/numbers, USING override, and channel
                std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
                for (size_t pi = 0; pi < items.size(); ++pi) {
                    const bool last = (pi + 1 == items.size());
                    const Expr* v = items[pi];
                    if (isStringExpr(v)) {
                        auto sptr = emitExpr(out, v, currLineLabel);
                        std::string defFmt = nextTemp(); { const char* sym = last ? "@.fmt_str" : "@.fmt_str_sp"; std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", defFmt, sym); out << ir << Symbols::LF; }
                        std::string useFmt = defFmt;
                        if (pr->format) useFmt = emitExpr(out, pr->format.get(), currLineLabel);
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                            { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fh, useFmt, sptr); out << ir << Symbols::LF; }
                        } else {
                            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", useFmt, sptr); out << ir << Symbols::LF; }
                            std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                            std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, useFmt, sptr); out << irn << Symbols::LF; }
                            std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                            { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                        }
                    } else {
                        bool nextStartsWithSpace = false;
                        if (!last && (pi + 1) < items.size()) {
                            if (const auto* ns = dyn_cast<const StringExpr>(items[pi + 1])) { if (!ns->value.empty() && ns->value.front() == ' ') nextStartsWithSpace = true; }
                        }
                        auto val = emitExpr(out, v, currLineLabel);
                        std::string fmtF = nextTemp(); { const char* sym = last ? "@.fmt_num" : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp"); std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, sym); out << ir << Symbols::LF; }
                        std::string fmtI = nextTemp(); { const char* symI = last ? "@.fmt_int" : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp"); std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, symI); out << ir << Symbols::LF; }
                        if (pr->format) {
                            std::string useFmt = emitExpr(out, pr->format.get(), currLineLabel);
                            std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", iv, val); out << ir << Symbols::LF; }
                            std::string dv = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", dv, iv); out << ir << Symbols::LF; }
                            std::string isInt = nextTemp(); { std::string ir = std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val); out << ir << Symbols::LF; }
                            std::string intLbl = currLineLabel + std::string("_print_int_") + std::to_string(++localCounter);
                            std::string fltLbl = currLineLabel + std::string("_print_flt_") + std::to_string(localCounter);
                            std::string contLbl = currLineLabel + std::string("_print_cont_") + std::to_string(localCounter);
                            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl); out << ir << Symbols::LF; }
                            out << intLbl << ":" << Symbols::LF;
                            if (pr->channel >= 1) {
                                std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                                std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                                { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fh, useFmt, iv); out << ir << Symbols::LF; }
                            } else {
                                { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", useFmt, iv); out << ir << Symbols::LF; }
                            }
                            { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                            out << fltLbl << ":" << Symbols::LF;
                            if (pr->channel >= 1) {
                                std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                                std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                                { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, useFmt, val); out << ir << Symbols::LF; }
                            } else {
                                { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", useFmt, val); out << ir << Symbols::LF; }
                            }
                            { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                            out << contLbl << ":" << Symbols::LF;
                        } else {
                            std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", iv, val); out << ir << Symbols::LF; }
                            std::string dv = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", dv, iv); out << ir << Symbols::LF; }
                            std::string isInt = nextTemp(); { std::string ir = std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val); out << ir << Symbols::LF; }
                            std::string intLbl = currLineLabel + std::string("_print_int_") + std::to_string(++localCounter);
                            std::string fltLbl = currLineLabel + std::string("_print_flt_") + std::to_string(localCounter);
                            std::string contLbl = currLineLabel + std::string("_print_cont_") + std::to_string(localCounter);
                            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl); out << ir << Symbols::LF; }
                            out << intLbl << ":" << Symbols::LF;
                            if (pr->channel >= 1) {
                                std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                                std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                                { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fh, fmtI, iv); out << ir << Symbols::LF; }
                            } else {
                                { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, iv); out << ir << Symbols::LF; }
                                std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                                std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i64 {})", n, sbuf, fmtI, iv); out << irn << Symbols::LF; }
                                std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                                { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                            }
                            { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                            out << fltLbl << ":" << Symbols::LF;
                            if (pr->channel >= 1) {
                                std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                                std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                                { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, fmtF, val); out << ir << Symbols::LF; }
                            } else {
                                { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmtF, val); out << ir << Symbols::LF; }
                                std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                                std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})", n, sbuf, fmtF, val); out << irn << Symbols::LF; }
                                std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                                { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                            }
                            { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                            out << contLbl << ":" << Symbols::LF;
                        }
                    }
                }
            } else if (auto aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
                // ELSE: numeric or string array element assignment with bounds checks
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
                std::string doLbl = currLineLabel + std::string("_arr_ok_") + std::to_string(++localCounter);
                std::string errLbl = currLineLabel + std::string("_arr_err_") + std::to_string(localCounter);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl); out << ir << Symbols::LF; }
                out << errLbl << ":" << Symbols::LF;
                { std::string ir = std::format("  store i32 9, ptr @gwb_err_code"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 0, ptr @gwb_resume_stmt"); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
                // Mirror error into ERR/ERL
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
            } else if (auto ib2 = dyn_cast<IfBlockStmt>(s.get())) {
                emitIfBlock(out, ib2, currLineLabel, localCounter);
            } else if (auto is = dyn_cast<IfStmt>(s.get())) {
                auto be2 = dyn_cast<BinaryExpr>(is->cond.get());
                if (!be2 || (be2->op != BinaryOp::Eq && be2->op != BinaryOp::Ne && be2->op != BinaryOp::Lt && be2->op != BinaryOp::Le && be2->op != BinaryOp::Gt && be2->op != BinaryOp::Ge)) {
                    throw CodeGenError("IF condition must be a comparison");
                }
                std::string cond2 = emitComparison(out, be2);
                std::string contLbl2 = currLineLabel + std::string("_cont_") + std::to_string(++localCounter);
                std::string irb2 = std::format("  br i1 {}, label %{}, label %{}", cond2, lineLabelName(is->targetLine), contLbl2);
                out << irb2 << Symbols::LF; log() << "line " << currentLine_ << " IfStmt -> " << irb2 << Symbols::LF;
                out << contLbl2 << ":" << Symbols::LF;
        } else if (auto ws = dyn_cast<WhileStmt>(s.get())) {
            emitWhile(out, ws, currLineLabel, localCounter);
            } else if (auto rz = dyn_cast<RandomizeStmt>(s.get())) {
                if (rz->seed) {
                    auto val = emitExpr(out, rz->seed.get(), currLineLabel);
                    std::string si = nextTemp(); { std::string ir = "  "; ir += si; ir += " = fptosi double "; ir += val; ir += " to i64"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Randomize fptosi -> " << ir << Symbols::LF; }
                    { std::string ir = "  call void @srand48(i64 "; ir += si; ir += ")"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Randomize srand48 -> " << ir << Symbols::LF; }
                } else {
                    std::string t = nextTemp(); { std::string ir = "  "; ir += t; ir += " = call i64 @time(ptr null)"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Randomize time -> " << ir << Symbols::LF; }
                    { std::string ir = "  call void @srand48(i64 "; ir += t; ir += ")"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Randomize srand48(time) -> " << ir << Symbols::LF; }
                }
            } else if (auto og = dyn_cast<OnGotoStmt>(s.get())) {
                std::string idx = emitExpr(out, og->index.get(), currLineLabel);
                std::string idxi32 = nextTemp(); { std::string ir = "  "; ir += idxi32; ir += " = fptosi double "; ir += idx; ir += " to i32"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else OnGoto fptosi -> " << ir << Symbols::LF; }
                std::string contLbl = currLineLabel + std::string("_on_cont_") + std::to_string(++localCounter);
                {
                    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < og->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << lineLabelName(og->targets[i]);
                    ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else OnGoto switch -> " << ir.str() << Symbols::LF;
                }
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ogs = dyn_cast<OnGosubStmt>(s.get())) {
                std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
                std::string idxi32 = nextTemp(); { std::string ir = "  "; ir += idxi32; ir += " = fptosi double "; ir += idx; ir += " to i32"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else OnGosub fptosi -> " << ir << Symbols::LF; }
                std::string contLbl = currLineLabel + std::string("_on_gs_cont_") + std::to_string(++localCounter);
                std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
                for (size_t i = 0; i < ogs->targets.size(); ++i) entryLbls.push_back(currLineLabel + std::string("_on_gs_entry_") + std::to_string(localCounter) + std::string("_") + std::to_string(i+1));
                {
                    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < ogs->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << entryLbls[i];
                    ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else OnGosub switch -> " << ir.str() << Symbols::LF;
                }
                for (size_t i = 0; i < ogs->targets.size(); ++i) emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
                out << contLbl << ":" << Symbols::LF;
            } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Return -> " << ir << Symbols::LF;
            } else if (isa<EndStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else End -> " << ir << Symbols::LF;
            } else if (isa<StopStmt>(s.get())) {
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_); out << ir << Symbols::LF; }
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Stop -> break+exit" << Symbols::LF;
            } else if (isa<SystemStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else System -> " << ir << Symbols::LF;
            } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine); out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Goto -> " << ir << Symbols::LF;
            } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
                std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
                std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
                out << "  br label %" << entryLbl << Symbols::LF;
                emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
                if (ins->promptLiteral || ins->promptVar) {
                    std::string pstr;
                    if (ins->promptLiteral) {
                        if (strLiteralId_.contains(*ins->promptLiteral)) {
                            int id = strLiteralId_[*ins->promptLiteral];
                            pstr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", pstr, globalStringName(id)); out << ir << Symbols::LF; }
                        }
                    } else {
                        ensureVarAllocated(out, *ins->promptVar);
                        pstr = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", pstr, varAllocaName_[*ins->promptVar]); out << ir << Symbols::LF; }
                        std::string safe = nextTemp(); { std::string ir = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, pstr); out << ir << Symbols::LF; } pstr = safe;
                    }
                    if (!pstr.empty()) {
                        std::string fmtS = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_str_sp, i64 0", fmtS); out << ir << Symbols::LF; }
                        { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmtS, pstr); out << ir << Symbols::LF; }
                    }
                }
                for (const auto& vname : ins->variables) {
                    ensureVarAllocated(out, vname);
                    std::string fmt = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Input -> " << ir1 << Symbols::LF; }
                    std::string tmp = nextTemp(); { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
                    { std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Input -> " << ir2 << Symbols::LF; }
                    std::string dv = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
                    storeNumberToVar(out, vname, dv);
                }
            } else {
                throw CodeGenError(std::string("Unsupported statement in IF body (ELSE): ")
                    + nodeName(s.get()));
            }
        }
        if (!elseTerminated) out << "  br label %" << endLbl << Symbols::LF;
    }

    out << endLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
