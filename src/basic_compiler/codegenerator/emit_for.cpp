// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include <format>
#include <sstream>
#include <cmath>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitFor
 * Inputs:
 *  - out: IR stream
 *  - fs: ForStmt node
 *  - currLineLabel: label base for naming blocks
 *  - localCounter: reference counter to make unique labels
 * Outputs:
 *  - void
 * Theory of operation:
 *  - Emits a standard counted FOR loop structure: init, cond, body, inc,
 *    end. Uses double precision arithmetic and inclusive end condition.
 */
void CodeGenerator::emitFor(std::ostringstream& out, const ForStmt* fs, const std::string& currLineLabel, int& localCounter) {
    
    std::string loopId = std::to_string(++localCounter);
    std::string condLbl = currLineLabel; condLbl += "_for_cond"; condLbl += loopId;
    std::string bodyLbl = currLineLabel; bodyLbl += "_for_body"; bodyLbl += loopId;
    std::string incLbl  = currLineLabel; incLbl  += "_for_inc";  incLbl  += loopId;
    std::string endLbl  = currLineLabel; endLbl  += "_for_end";  endLbl  += loopId;

    ensureVarAllocated(out, fs->var);
    {
        std::string startReg = emitExpr(out, fs->start.get(), currLineLabel); // double
        // Store to the loop variable with correct underlying type
        storeNumberToVar(out, fs->var, startReg);
        std::string ir2 = std::format("  br label %{}", condLbl);
        out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " ForStmt -> " << ir2 << Symbols::LF;
    }

    out << condLbl << ":" << Symbols::LF;
    std::string curVal = nextTemp(); // as double for comparisons
    {
        // Load the current loop variable and widen to double as needed
        switch (numKindOf(fs->var)) {
            case NumKind::Int16: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load i16, ptr {}", l, varAllocaName_[fs->var]); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = sitofp i16 {} to double", curVal, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Long32: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load i32, ptr {}", l, varAllocaName_[fs->var]); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = sitofp i32 {} to double", curVal, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Single: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load float, ptr {}", l, varAllocaName_[fs->var]); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = fpext float {} to double", curVal, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Double: {
                std::string ir = std::format("  {} = load double, ptr {}", curVal, varAllocaName_[fs->var]);
                out << ir << Symbols::LF;
                break;
            }
        }
        log() << "line " << currentLine_ << " ForStmt cond load (->double)" << Symbols::LF;
    }
    {
        // Evaluate end and step for condition decision
        std::string endReg = emitExpr(out, fs->end.get(), currLineLabel);
        std::string stepReg = fs->step ? emitExpr(out, fs->step.get(), currLineLabel) : std::string("1.0");
        std::string isNeg = nextTemp();
        { std::string ir = std::format("  {} = fcmp olt double {}, 0.0", isNeg, stepReg); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt step<0 -> " << ir << Symbols::LF; }
        std::string condLe = nextTemp();
        { std::string ir = std::format("  {} = fcmp ole double {}, {}", condLe, curVal, endReg); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt cond <= -> " << ir << Symbols::LF; }
        std::string condGe = nextTemp();
        { std::string ir = std::format("  {} = fcmp oge double {}, {}", condGe, curVal, endReg); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt cond >= -> " << ir << Symbols::LF; }
        std::string cond = nextTemp();
        { std::string ir = std::format("  {} = select i1 {}, i1 {}, i1 {}", cond, isNeg, condGe, condLe); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt select cond -> " << ir << Symbols::LF; }
        std::string br = std::format("  br i1 {}, label %{}, label %{}", cond, bodyLbl, endLbl);
        out << br << Symbols::LF; log() << "line " << currentLine_ << " ForStmt branch -> " << br << Symbols::LF;
    }

    out << bodyLbl << ":" << Symbols::LF;
    bool forTerminated = false;
    for (const auto& s : fs->body) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
                std::string ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body Assign$ -> " << ir << Symbols::LF;
            } else {
                // Numeric assignment: cast to destination storage and store
                storeNumberToVar(out, asg->name, val);
            }
        } else if (auto pr = dyn_cast<PrintStmt>(s.get())) {
            // Match emit_line_block dynamic integer/float PRINT semantics
            std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
            for (size_t pi = 0; pi < items.size(); ++pi) {
                const bool last = (pi + 1 == items.size());
                const Expr* v = items[pi];
                if (isStringExpr(v)) {
                    auto sptr = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp();
                    { std::string ir2 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body Print -> " << ir2 << Symbols::LF; }
                    // Optional USING override
                    std::string useFmt = fmt;
                    if (pr->format) { useFmt = emitExpr(out, pr->format.get(), currLineLabel); }
                    if (pr->channel >= 1) {
                        std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                        std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                        { std::string ir3 = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fh, useFmt, sptr); out << ir3 << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body Print fprintf -> " << ir3 << Symbols::LF; }
                    } else {
                        { std::string ir3 = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", useFmt, sptr); out << ir3 << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body Print printf -> " << ir3 << Symbols::LF; }
                        // Mirror to virtual screen
                        std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                        std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, useFmt, sptr); out << irn << Symbols::LF; }
                        std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                        { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                    }
                } else {
                    // Fast path: compile-time constant number
                    if (const auto cnum = dyn_cast<const NumberExpr>(v)) {
                        const double cv = cnum->value;
                        const bool isIntegral = (std::floor(cv) == cv);
                        bool nextStartsWithSpace = false;
                        if (!last && (pi + 1) < items.size()) {
                            if (const auto* ns = dyn_cast<const StringExpr>(items[pi + 1])) {
                                if (!ns->value.empty() && ns->value.front() == ' ') nextStartsWithSpace = true;
                            }
                        }
                        if (isIntegral) {
                            std::string fmtI = nextTemp();
                            const char* symI = last ? "@.fmt_int" : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp");
                            { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, symI); out << ir1 << Symbols::LF; }
                            long long iv = static_cast<long long>(cv);
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
                            continue;
                        } else {
                            std::string fmtF = nextTemp();
                            const char* symF = last ? "@.fmt_num" : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp");
                            { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, symF); out << ir1 << Symbols::LF; }
                            if (pr->channel >= 1) {
                                std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                                std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                                { std::string ir = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {:.6f})", fh, fmtF, cv); out << ir << Symbols::LF; }
                            } else {
                                { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {:.6f})", fmtF, cv); out << ir << Symbols::LF; }
                                std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                                std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {:.6f})", n, sbuf, fmtF, cv); out << irn << Symbols::LF; }
                                std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                                { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                            }
                            continue;
                        }
                    }
                    // General case: dynamic integer detection
                    auto val = emitExpr(out, v, currLineLabel);
                    bool hasOverride = (pr->format != nullptr);
                    bool nextStartsWithSpace = false;
                    if (!last && (pi + 1) < items.size()) {
                        if (const auto* ns = dyn_cast<const StringExpr>(items[pi + 1])) {
                            if (!ns->value.empty() && ns->value.front() == ' ') nextStartsWithSpace = true;
                        }
                    }
                    std::string fmtF = nextTemp();
                    { const char* sym = last ? "@.fmt_num" : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp"); std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, sym); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body Print fmtF -> " << ir1 << Symbols::LF; }
                    std::string fmtI = nextTemp();
                    { const char* symI = last ? "@.fmt_int" : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp"); std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, symI); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body Print fmtI -> " << ir1 << Symbols::LF; }
                    if (hasOverride) {
                        std::string useFmt = emitExpr(out, pr->format.get(), currLineLabel);
                        // Dynamic int/float split while honoring override
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
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fh, useFmt, iv); out << ir2 << Symbols::LF; }
                        } else {
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", useFmt, iv); out << ir2 << Symbols::LF; }
                        }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        out << fltLbl << ":" << Symbols::LF;
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, useFmt, val); out << ir2 << Symbols::LF; }
                        } else {
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", useFmt, val); out << ir2 << Symbols::LF; }
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
            }
        } else if (isa<StopStmt>(s.get())) {
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_); out << ir << Symbols::LF; }
            out << std::format("  br label %exit") << Symbols::LF;
            forTerminated = true; break;
        } else if (isa<SystemStmt>(s.get())) {
            out << std::format("  br label %exit") << Symbols::LF;
            forTerminated = true; break;
        } else if (auto mid = dyn_cast<MidAssignStmt>(s.get())) {
            // Mirror MID$ lowering from line-block/if-block emitters
            std::string dest;
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
                std::string doLbl = currLineLabel + std::string("_mid_ok_") + std::to_string(++localCounter);
                std::string endLbl2 = currLineLabel + std::string("_mid_end_") + std::to_string(localCounter);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", anyBad, endLbl2, doLbl); out << ir << Symbols::LF; }
                out << doLbl << ":" << Symbols::LF;
                // Compute destination element pointer
                std::vector<long long> extents; extents.reserve(dims.size());
                for (size_t di = 0; di < dims.size(); ++di) { long long e = static_cast<long long>(dims[di]) - optionBase_ + 1; if (e < 0) e = 0; extents.push_back(e); }
                std::vector<long long> strides(dims.size(), 1);
                for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) { strides[di] = strides[di + 1] * extents[di + 1]; }
                std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
                for (const auto& ii : idxI64s) { std::string a = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", a, ii, optionBase_); out << ir << Symbols::LF; } adjs.push_back(a); }
                std::string lin = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]); out << ir << Symbols::LF; }
                for (size_t di = 1; di < adjs.size(); ++di) { std::string t = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", t, adjs[di], strides[di]); out << ir << Symbols::LF; } std::string s2 = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", s2, lin, t); out << ir << Symbols::LF; } lin = s2; }
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin); out << ir << Symbols::LF; }
                std::string dptr = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", dptr, elem); out << ir << Symbols::LF; }
                dest = dptr;
                // End label when OOB
                { std::string ir = std::format("  br label %{}", endLbl2); out << ir << Symbols::LF; }
                out << endLbl2 << ":" << Symbols::LF;
            } else {
                ensureVarAllocated(out, mid->name);
                dest = varAllocaName_[mid->name];
            }
            // Compute offsets/length
            std::string off = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off, emitExpr(out, mid->start.get(), currLineLabel)); out << ir << Symbols::LF; }
            std::string dlen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", dlen, dest); out << ir << Symbols::LF; }
            std::string n = nextTemp(); { std::string ir = std::format("  {} = {}", n, (mid->len ? std::string("fptosi double ") + emitExpr(out, mid->len.get(), currLineLabel) + " to i64" : std::string("call i64 @strlen(ptr ") + emitExpr(out, mid->value.get(), currLineLabel) + ")")); out << ir << Symbols::LF; }
            std::string src = nextTemp(); { std::string ir = std::format("  {} = {}", src, (mid->len ? std::string("getelementptr inbounds i8, ptr ") + emitExpr(out, mid->value.get(), currLineLabel) + ", i64 0" : std::string("getelementptr inbounds i8, ptr ") + emitExpr(out, mid->value.get(), currLineLabel) + ", i64 0")); out << ir << Symbols::LF; }
            std::string negOff = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", negOff, off); out << ir << Symbols::LF; }
            std::string geLen = nextTemp(); { std::string ir = std::format("  {} = icmp sge i64 {}, {}", geLen, off, dlen); out << ir << Symbols::LF; }
            std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, negOff, geLen); out << ir << Symbols::LF; }
            std::string doLbl = currLineLabel + std::string("_mid_do_") + std::to_string(++localCounter);
            std::string endLbl2b = currLineLabel + std::string("_mid_end_") + std::to_string(localCounter);
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", bad, endLbl2b, doLbl); out << ir << Symbols::LF; }
            out << doLbl << ":" << Symbols::LF;
            std::string avail = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", avail, dlen, off); out << ir << Symbols::LF; }
            std::string n_lt_av = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", n_lt_av, n, avail); out << ir << Symbols::LF; }
            std::string m1 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", m1, n_lt_av, n, avail); out << ir << Symbols::LF; }
            std::string m1_lt_s = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", m1_lt_s, m1, dlen); out << ir << Symbols::LF; }
            std::string m2 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", m2, m1_lt_s, m1, dlen); out << ir << Symbols::LF; }
            std::string dst = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", dst, dest, off); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", dst, src, m2); out << ir << Symbols::LF; }
            { std::string ir = std::format("  br label %{}", endLbl2b); out << ir << Symbols::LF; }
            out << endLbl2b << ":" << Symbols::LF;
        } else if (auto og = dyn_cast<OnGotoStmt>(s.get())) {
            std::string idx = emitExpr(out, og->index.get(), currLineLabel);
            std::string idxi32 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", idxi32, idx); out << ir << Symbols::LF; }
            std::string contLbl = currLineLabel + std::string("_on_cont_") + std::to_string(++localCounter);
            {
                std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                for (size_t i = 0; i < og->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << lineLabelName(og->targets[i]);
                ir << " ]"; out << ir.str() << Symbols::LF; }
            out << contLbl << ":" << Symbols::LF;
        } else if (auto ogs = dyn_cast<OnGosubStmt>(s.get())) {
            std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
            std::string idxi32 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", idxi32, idx); out << ir << Symbols::LF; }
            std::string contLbl = currLineLabel + std::string("_on_gs_cont_") + std::to_string(++localCounter);
            std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
            for (size_t i = 0; i < ogs->targets.size(); ++i) entryLbls.push_back(currLineLabel + std::string("_on_gs_entry_") + std::to_string(localCounter) + std::string("_") + std::to_string(i+1));
            {
                std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                for (size_t i = 0; i < ogs->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << entryLbls[i];
                ir << " ]"; out << ir.str() << Symbols::LF; }
            for (size_t i = 0; i < ogs->targets.size(); ++i) emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
            out << contLbl << ":" << Symbols::LF;
        } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine); out << ir << Symbols::LF; forTerminated = true; break;
        } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
            std::string contLbl = currLineLabel + std::string("_gosub_cont") + std::to_string(++localCounter);
            std::string entryLbl = currLineLabel + std::string("_gosub_entry") + std::to_string(localCounter);
            out << "  br label %" << entryLbl << Symbols::LF;
            emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
            out << contLbl << ":" << Symbols::LF;
        } else if (auto aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
            const auto &dims = arrayDims_[aaset->name];
            long long total = 1; for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); if (ext < 0) ext = 0; total *= ext; }
            std::vector<std::string> idxI64s; idxI64s.reserve(aaset->indices.size());
            std::vector<std::string> bads; bads.reserve(aaset->indices.size());
            for (size_t di = 0; di < aaset->indices.size(); ++di) {
                std::string idxReg = emitExpr(out, aaset->indices[di].get(), currLineLabel);
                std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body idx -> " << ir << Symbols::LF; }
                idxI64s.push_back(idxI64);
                std::string ltBase = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI64, optionBase_); out << ir << Symbols::LF; }
                std::string gtUb = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI64, dims[di]); out << ir << Symbols::LF; }
                std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb); out << ir << Symbols::LF; }
                bads.push_back(bad);
            }
            std::string anyBad = bads[0];
            for (size_t i = 1; i < bads.size(); ++i) { std::string nb = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]); out << ir << Symbols::LF; } anyBad = nb; }
            std::string doLbl = currLineLabel + std::string("_for_arr_ok_") + std::to_string(++localCounter);
            std::string errLbl = currLineLabel + std::string("_for_arr_err_") + std::to_string(localCounter);
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
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body gep$ -> " << ir << Symbols::LF; }
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                { std::string ir = std::format("  store ptr {}, ptr {}", val, elem); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body store$ -> " << ir << Symbols::LF; }
            } else {
                ensureArrayAllocated(out, aaset->name, static_cast<int>(total));
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, total, arrayElemType(aaset->name), base, lin); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body gep -> " << ir << Symbols::LF; }
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                storeNumberToArrayElem(out, aaset->name, elem, val);
            }
        } else if (auto w = dyn_cast<WhileStmt>(s.get())) {
            emitWhile(out, w, currLineLabel, localCounter);
        } else {
            throw CodeGenError("Unsupported statement in FOR body");
        }
    }
    if (!forTerminated) out << "  br label %" << incLbl << Symbols::LF;

    if (!forTerminated) {
        out << incLbl << ":" << Symbols::LF;
        std::string stepReg = fs->step ? emitExpr(out, fs->step.get(), currLineLabel) : std::string("1.0");
        std::string vcur = nextTemp();
        // Load current value into vcur as double
        switch (numKindOf(fs->var)) {
            case NumKind::Int16: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load i16, ptr {}", l, varAllocaName_[fs->var]); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = sitofp i16 {} to double", vcur, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Long32: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load i32, ptr {}", l, varAllocaName_[fs->var]); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = sitofp i32 {} to double", vcur, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Single: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load float, ptr {}", l, varAllocaName_[fs->var]); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = fpext float {} to double", vcur, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Double: {
                std::string ir = std::format("  {} = load double, ptr {}", vcur, varAllocaName_[fs->var]); out << ir << Symbols::LF; break;
            }
        }
        std::string vnext = nextTemp();
        { std::string ir = std::format("  {} = fadd double {}, {}", vnext, vcur, stepReg); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt inc add -> " << ir << Symbols::LF; }
        // Store back to the variable with correct type
        storeNumberToVar(out, fs->var, vnext);
        { std::string ir = std::format("  br label %{}", condLbl); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt -> " << ir << Symbols::LF; }
    }

    out << endLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
