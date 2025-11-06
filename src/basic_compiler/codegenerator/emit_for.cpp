// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
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
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, useFmt, val); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body Print fprintf (override) -> " << ir2 << Symbols::LF; }
                        } else {
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", useFmt, val); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body Print printf (override) -> " << ir2 << Symbols::LF; }
                            std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                            std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})", n, sbuf, useFmt, val); out << irn << Symbols::LF; }
                            std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                            { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                        }
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
        } else if (auto aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
            const int len = arraySizes_[aaset->name];
            std::string idxReg = emitExpr(out, aaset->index.get(), currLineLabel);
            std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body idx -> " << ir << Symbols::LF; }
            if (isStringArrayNameCG(aaset->name)) {
                ensureStringArrayAllocated(out, aaset->name, len);
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body gep$ -> " << ir << Symbols::LF; }
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                { std::string ir = std::format("  store ptr {}, ptr {}", val, elem); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body store$ -> " << ir << Symbols::LF; }
            } else {
                ensureArrayAllocated(out, aaset->name, len);
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body gep -> " << ir << Symbols::LF; }
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                { std::string ir = std::format("  store double {}, ptr {}", val, elem); out << ir << Symbols::LF; log() << "line " << currentLine_ << " ForStmt body store -> " << ir << Symbols::LF; }
            }
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
