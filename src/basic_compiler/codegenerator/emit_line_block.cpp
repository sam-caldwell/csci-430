// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/OpenStmt.h"
#include "basic_compiler/ast/CloseStmt.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/DefSegStmt.h"
#include "basic_compiler/ast/BloadStmt.h"
#include "basic_compiler/ast/BsaveStmt.h"
#include "basic_compiler/ast/PokeStmt.h"
#include "basic_compiler/ast/CallAbsStmt.h"
#include "basic_compiler/ast/DefUsrStmt.h"
#include "basic_compiler/ast/ChdirStmt.h"
#include "basic_compiler/ast/ColorStmt.h"
#include "basic_compiler/ast/ScreenStmt.h"
#include "basic_compiler/ast/CircleStmt.h"
#include "basic_compiler/ast/ClearStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/ResumeStmt.h"
#include "basic_compiler/ast/ErrorStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include <sstream>
#include <format>
#include <cmath>

namespace gwbasic {
    void CodeGenerator::emitLineBlock(std::ostringstream &out, const Line &line, int lineIndex, int lastIndex) {
        /*
         * Function: CodeGenerator::emitLineBlock
         * Inputs:
         *  - out: IR stream
         *  - line: AST line to emit
         *  - lineIndex/lastIndex: indices to compute fall-through label
         * Outputs:
         *  - void
         * Theory of operation:
         *  - Emits a basic block label for the line, then iterates statements,
         *    generating IR for assignments, PRINT, GOTO, GOSUB/RETURN, IF, INPUT,
         *    and inline FOR loops. Terminates with a branch to the next line or
         *    %exit on END/RETURN/GOTO.
         */
        currentLine_ = line.number;
        out << lineLabelName(line.number) << ":" << Symbols::LF;
        { std::ostringstream m; m << "begin line " << currentLine_; log() << m.str() << Symbols::LF; }
        int localContCounter = 0;
        auto nextLabel = (lineIndex < lastIndex) ? lineLabelName(lineNumbers_[lineIndex + 1]) : std::string("exit");
        bool terminated = false;
        // If this line is the active error handler line and we're not currently
        // in a handler context, skip over the handler region (to the first
        // non-handler line after the first RESUME). This prevents normal
        // fallthrough from entering the handler code.
        {
            auto itSkip = handlerSkipAfter_.find(line.number);
            if (itSkip != handlerSkipAfter_.end()) {
                std::string tl = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_err_trap_line", tl); out << ir << Symbols::LF; }
                std::string isThis = nextTemp(); { std::string ir = std::format("  {} = icmp eq i32 {}, {}", isThis, tl, line.number); out << ir << Symbols::LF; }
                std::string contLbl = lineLabelName(line.number) + std::string("_hdlr_cont_") + std::to_string(++localContCounter);
                std::string chkLbl = lineLabelName(line.number) + std::string("_hdlr_chk_") + std::to_string(localContCounter);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isThis, chkLbl, contLbl); out << ir << Symbols::LF; }
                out << chkLbl << ":" << Symbols::LF;
                std::string ih = nextTemp(); { std::string ir = std::format("  {} = load i1, ptr @gwb_in_handler", ih); out << ir << Symbols::LF; }
                std::string notIH = nextTemp(); { std::string ir = std::format("  {} = icmp eq i1 {}, false", notIH, ih); out << ir << Symbols::LF; }
                std::string skipLbl = lineLabelName(line.number) + std::string("_hdlr_skip_") + std::to_string(localContCounter);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", notIH, skipLbl, contLbl); out << ir << Symbols::LF; }
                out << skipLbl << ":" << Symbols::LF;
                // Skip to after-handler destination or exit
                if (itSkip->second >= 0) {
                    std::string ir = std::format("  br label %{}", lineLabelName(itSkip->second));
                    out << ir << Symbols::LF;
                } else {
                    std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF;
                }
                out << contLbl << ":" << Symbols::LF;
            }
        }
        for (size_t i = 0; i < line.statements.size(); ++i) {
            const auto &st = line.statements[i];
            const int stmtIndex = static_cast<int>(i) + 1;
            // Statement boundary (no standalone label to keep IR blocks well-formed)
            bool stmtTerminates = false;
            if (auto asg = dyn_cast<AssignStmt>(st.get())) {
                std::string val = emitExpr(out, asg->value.get(), "");
                if (isStringVarNameCG(asg->name)) {
                    std::string ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
                    out << ir << Symbols::LF;
                    { std::ostringstream m; m << "line " << currentLine_ << ' ' << nodeName(st.get()) << " -> " << ir; log() << m.str() << Symbols::LF; }
                } else {
                    storeNumberToVar(out, asg->name, val);
                }
            } else if (auto aaset = dyn_cast<ArrayAssignStmt>(st.get())) {
                // A(i) = expr
                const int len = arraySizes_[aaset->name];
                ensureArrayAllocated(out, aaset->name, len);
                std::string base = arrayAllocaName_[aaset->name];
                std::string idxReg = emitExpr(out, aaset->index.get(), "");
                std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Array idx -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Array gep -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string val = emitExpr(out, aaset->value.get(), "");
                { std::string ir = std::format("  store double {}, ptr {}", val, elem); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Array store -> " << ir; log() << m.str() << Symbols::LF; } }
            } else if (auto pr = dyn_cast<PrintStmt>(st.get())) {
                std::vector<const Expr *> items;
                if (pr->value) items.push_back(pr->value.get());
                for (const auto &v: pr->more) items.push_back(v.get());
                for (size_t pi = 0; pi < items.size(); ++pi) {
                    const bool last = (pi + 1 == items.size());
                    const Expr *v = items[pi];
                    if (isStringExpr(v)) {
                        auto sptr = emitExpr(out, v, "");
                        std::string fmt = nextTemp();
                        { std::string ir2 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")); out << ir2 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir2; log() << m.str() << Symbols::LF; } }
                        // Optional USING override
                        std::string useFmt = fmt;
                        if (pr->format) { useFmt = emitExpr(out, pr->format.get(), ""); }
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                            { std::string ir3 = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fh, useFmt, sptr); out << ir3 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt fprintf -> " << ir3; log() << m.str() << Symbols::LF; } }
                        } else {
                            { std::string ir3 = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", useFmt, sptr); out << ir3 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt printf -> " << ir3; log() << m.str() << Symbols::LF; } }
                            // Mirror to virtual screen using snprintf and helper
                            std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                            std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, useFmt, sptr); out << irn << Symbols::LF; }
                            std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                            { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                        }
                    } else {
                        // Fast path: compile-time constant number? Emit direct format without runtime fcmp
                        if (const auto cnum = dyn_cast<const NumberExpr>(v)) {
                            const double cv = cnum->value;
                            const bool isIntegral = (std::floor(cv) == cv);
                            // Determine spacing policy for auto formats
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
                        auto val = emitExpr(out, v, "");
                        // USING override? If provided, honor it with double formatting
                        bool hasOverride = (pr->format != nullptr);
                        // Determine spacing policy for auto formats
                        bool nextStartsWithSpace = false;
                        if (!last && (pi + 1) < items.size()) {
                            if (const auto* ns = dyn_cast<const StringExpr>(items[pi + 1])) {
                                if (!ns->value.empty() && ns->value.front() == ' ') nextStartsWithSpace = true;
                            }
                        }
                        // Build float and int format pointers
                        std::string fmtF = nextTemp();
                        {
                            const char* sym = last ? "@.fmt_num" : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp");
                            std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, sym);
                            out << ir1 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt fmtF -> " << ir1; log() << m.str() << Symbols::LF; }
                        }
                        std::string fmtI = nextTemp();
                        {
                            const char* symI = last ? "@.fmt_int" : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp");
                            std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, symI);
                            out << ir1 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt fmtI -> " << ir1; log() << m.str() << Symbols::LF; }
                        }
                        if (hasOverride) {
                            std::string useFmt = emitExpr(out, pr->format.get(), "");
                            if (pr->channel >= 1) {
                                std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << Symbols::LF; }
                                std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << Symbols::LF; }
                                { std::string ir2 = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, useFmt, val); out << ir2 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt fprintf (override) -> " << ir2; log() << m.str() << Symbols::LF; } }
                            } else {
                                { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", useFmt, val); out << ir2 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt printf (override) -> " << ir2; log() << m.str() << Symbols::LF; } }
                                // Mirror to virtual screen using snprintf
                                std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << Symbols::LF; }
                                std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})", n, sbuf, useFmt, val); out << irn << Symbols::LF; }
                                std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << Symbols::LF; }
                                { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << Symbols::LF; }
                            }
                        } else {
                            // Dynamic integer detection: compare value to its integer cast
                            std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", iv, val); out << ir << Symbols::LF; }
                            std::string dv = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", dv, iv); out << ir << Symbols::LF; }
                            std::string isInt = nextTemp(); { std::string ir = std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val); out << ir << Symbols::LF; }
                            std::string intLbl = lineLabelName(line.number) + std::string("_print_int_") + std::to_string(++localContCounter);
                            std::string fltLbl = lineLabelName(line.number) + std::string("_print_flt_") + std::to_string(localContCounter);
                            std::string contLbl = lineLabelName(line.number) + std::string("_print_cont_") + std::to_string(localContCounter);
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
            } else if (auto gt = dyn_cast<GotoStmt>(st.get())) {
                std::string ir = std::format("  br label %{}", lineLabelName(gt->targetLine));
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " GotoStmt -> " << ir; log() << m.str() << Symbols::LF; }
                stmtTerminates = true;
            } else if (auto gs = dyn_cast<GosubStmt>(st.get())) {
                std::string contLbl = lineLabelName(line.number);
                contLbl += "_gosub_cont";
                contLbl += std::to_string(++localContCounter);
                std::string entryLbl = lineLabelName(line.number);
                entryLbl += "_gosub_entry";
                entryLbl += std::to_string(localContCounter);
                out << "  br label %" << entryLbl << Symbols::LF;
                emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
                out << contLbl << ":" << Symbols::LF;
            } else if (auto is = dyn_cast<IfStmt>(st.get())) {
                auto be = dyn_cast<BinaryExpr>(is->cond.get());
                if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt && be->op !=
                            BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) {
                    throw CodeGenError("IF condition must be a comparison");
                }
                std::string cond = emitComparison(out, be);
                std::string contLbl = "line";
                contLbl += std::to_string(line.number);
                contLbl += "_cont";
                contLbl += std::to_string(++localContCounter);
                std::string ir = std::format("  br i1 {}, label %{}, label %{}", cond, lineLabelName(is->targetLine), contLbl);
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " IfStmt -> " << ir; log() << m.str() << Symbols::LF; }
                out << contLbl << ":" << Symbols::LF;
            } else if (auto og = dyn_cast<OnGotoStmt>(st.get())) {
                // Evaluate index and dispatch to one of the targets by 1-based index; default falls through
                std::string idx = emitExpr(out, og->index.get(), lineLabelName(line.number));
                std::string idxi32 = nextTemp();
                { std::string ir = std::format("  {} = fptosi double {} to i32", idxi32, idx); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " OnGoto fptosi -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string contLbl = lineLabelName(line.number) + std::string("_on_cont_") + std::to_string(++localContCounter);
                // Emit switch header
                {
                    std::ostringstream ir;
                    ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < og->targets.size(); ++i) {
                        ir << " i32 " << (i + 1) << ", label %" << lineLabelName(og->targets[i]);
                    }
                    ir << " ]";
                    out << ir.str() << Symbols::LF;
                    { std::ostringstream m; m << "line " << currentLine_ << " OnGoto switch -> " << ir.str(); log() << m.str() << Symbols::LF; }
                }
                // Continuation label for out-of-range/zero
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ogs = dyn_cast<OnGosubStmt>(st.get())) {
                // Evaluate index and dispatch to one of several subroutine entries
                std::string idx = emitExpr(out, ogs->index.get(), lineLabelName(line.number));
                std::string idxi32 = nextTemp();
                { std::string ir = std::format("  {} = fptosi double {} to i32", idxi32, idx); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " OnGosub fptosi -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string contLbl = lineLabelName(line.number) + std::string("_on_gs_cont_") + std::to_string(++localContCounter);
                // Build case entries and emit switch to per-case entry labels
                std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
                for (size_t i = 0; i < ogs->targets.size(); ++i) {
                    std::string el = lineLabelName(line.number) + std::string("_on_gs_entry_") + std::to_string(localContCounter) + std::string("_") + std::to_string(i+1);
                    entryLbls.push_back(el);
                }
                {
                    std::ostringstream ir;
                    ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < ogs->targets.size(); ++i) {
                        ir << " i32 " << (i + 1) << ", label %" << entryLbls[i];
                    }
                    ir << " ]";
                    out << ir.str() << Symbols::LF;
                    { std::ostringstream m; m << "line " << currentLine_ << " OnGosub switch -> " << ir.str(); log() << m.str() << Symbols::LF; }
                }
                // Emit each entry label and inline the subroutine
                for (size_t i = 0; i < ogs->targets.size(); ++i) {
                    emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
                }
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ib = dyn_cast<IfBlockStmt>(st.get())) {
                emitIfBlock(out, ib, lineLabelName(line.number), localContCounter);
            } else if (auto ws = dyn_cast<WhileStmt>(st.get())) {
                emitWhile(out, ws, lineLabelName(line.number), localContCounter);
            } else if (isa<EndStmt>(st.get())) {
                std::string ir = std::format("  br label %exit");
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " EndStmt -> " << ir; log() << m.str() << Symbols::LF; }
                terminated = true;
                break;
            } else if (isa<StopStmt>(st.get())) {
                // STOP: print break message and terminate
                std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF; }
                { std::ostringstream m; m << "line " << currentLine_ << " StopStmt -> break+exit"; log() << m.str() << Symbols::LF; }
                terminated = true;
                break;
            } else if (isa<SystemStmt>(st.get())) {
                // SYSTEM: terminate program (like END)
                std::string ir = std::format("  br label %exit");
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " SystemStmt -> " << ir; log() << m.str() << Symbols::LF; }
                terminated = true;
                break;
            } else if (auto ins = dyn_cast<InputStmt>(st.get())) {
                ensureVarAllocated(out, ins->name);
                // Read into a temporary double, then convert to variable's storage type
                std::string fmt = nextTemp();
                { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " InputStmt fmt -> " << ir1; log() << m.str() << Symbols::LF; } }
                std::string tmp = nextTemp();
                { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
                { std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp); out << ir2 << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " InputStmt scanf -> " << ir2; log() << m.str() << Symbols::LF; } }
                std::string dv = nextTemp();
                { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
                storeNumberToVar(out, ins->name, dv);
            } else if (auto fs = dyn_cast<ForStmt>(st.get())) {
                emitFor(out, fs, lineLabelName(line.number), localContCounter);
            } else if (auto rz = dyn_cast<RandomizeStmt>(st.get())) {
                // RANDOMIZE [expr]
                if (rz->seed) {
                    auto val = emitExpr(out, rz->seed.get(), "");
                    std::string si = nextTemp();
                    {
                        std::string ir = std::format("  {} = fptosi double {} to i64", si, val);
                        out << ir << Symbols::LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " Randomize fptosi -> " << ir; log() << m.str() << Symbols::LF; }
                    }
                    {
                        std::string ir = std::format("  call void @srand48(i64 {})", si);
                        out << ir << Symbols::LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " Randomize srand48 -> " << ir; log() << m.str() << Symbols::LF; }
                    }
                } else {
                    std::string t = nextTemp();
                    {
                        std::string ir = std::format("  {} = call i64 @time(ptr null)", t);
                        out << ir << Symbols::LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " Randomize time -> " << ir; log() << m.str() << Symbols::LF; }
                    }
                    {
                        std::string ir = std::format("  call void @srand48(i64 {})", t);
                        out << ir << Symbols::LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " Randomize srand48(time) -> " << ir; log() << m.str() << Symbols::LF; }
                    }
                }
            } else if (isa<ReturnStmt>(st.get())) {
                std::string ir = std::format("  br label %exit");
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " ReturnStmt -> " << ir; log() << m.str() << Symbols::LF; }
                stmtTerminates = true;
            } else if (auto cs = dyn_cast<CommonStmt>(st.get())) {
                // COMMON has no direct codegen effect in this compiler; treat as no-op.
                { std::ostringstream m; m << "line " << currentLine_ << " CommonStmt (no-op)"; log() << m.str() << Symbols::LF; }
            } else if (auto rn = dyn_cast<RunStmt>(st.get())) {
                // Reset all variables to zero/null and branch to first or specified line
                for (const auto &v: variables_) { if (varAllocaName_.contains(v)) resetVar(out, v); }
                int dest = rn->targetLine.has_value()
                               ? *rn->targetLine
                               : (lineNumbers_.empty() ? line.number : lineNumbers_.front());
                std::string ir = "  br label %";
                ir += lineLabelName(dest);
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " RunStmt branch -> " << ir; log() << m.str() << Symbols::LF; }
                terminated = true;
                break;
            } else if (auto ch = dyn_cast<ChainStmt>(st.get())) {
                // CHAIN: reset non-preserved variables/arrays and branch to target/first line
                if (!ch->all) {
                    // Preserve only variables declared COMMON before this line
                    const auto itCBL = commonBeforeLine_.find(line.number);
                    const std::set<std::string> emptySet;
                    const std::set<std::string>& preserve = (itCBL == commonBeforeLine_.end()) ? emptySet : itCBL->second;
                    for (const auto &v: variables_) {
                        if (preserve.contains(v)) continue; // preserve caller's COMMON only
                        auto it = varAllocaName_.find(v);
                        if (it == varAllocaName_.end()) continue;
                        resetVar(out, v);
                    }
                    // Also clear arrays not marked COMMON before this line
                    const auto itAB = arraysBeforeLine_.find(line.number);
                    const std::set<std::string> emptyArr;
                    const std::set<std::string>& aset = (itAB == arraysBeforeLine_.end()) ? emptyArr : itAB->second;
                    for (const auto &an : aset) {
                        if (preserve.contains(an)) continue; // preserve COMMON arrays
                        auto itLen = arraySizes_.find(an);
                        if (itLen == arraySizes_.end()) continue;
                        const int len = itLen->second;
                        ensureArrayAllocated(out, an, len);
                        std::string base = arrayAllocaName_[an];
                        for (int i = 0; i < len; ++i) {
                            std::string elem = nextTemp();
                            { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, i); out << ir << Symbols::LF; }
                            { std::string ir = std::format("  store double 0.0, ptr {}", elem); out << ir << Symbols::LF; }
                        }
                    }
                }
                int dest = ch->targetLine.has_value()
                               ? *ch->targetLine
                               : (lineNumbers_.empty() ? line.number : lineNumbers_.front());
                // Reset DATA pointer to the beginning of the destination program segment
                {
                    const int region = (dest / 1000) * 1000;
                    int dataStart = 0;
                    auto it = regionDataStartIdx_.find(region);
                    if (it != regionDataStartIdx_.end()) dataStart = it->second;
                    std::string ir = std::format("  store i32 {}, ptr @gwb_data_idx", dataStart);
                    out << ir << Symbols::LF;
                    { std::ostringstream m; m << "line " << currentLine_ << " ChainStmt data_idx -> " << ir; log() << m.str() << Symbols::LF; }
                }
                std::string ir = "  br label %";
                ir += lineLabelName(dest);
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " ChainStmt branch -> " << ir; log() << m.str() << Symbols::LF; }
                terminated = true;
                break;
            } else if (auto oeg = dyn_cast<OnErrorGotoStmt>(st.get())) {
                // Set trap line (0 disables)
                { std::string ir = std::format("  store i32 {}, ptr @gwb_err_trap_line", oeg->targetLine); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " OnErrorGoto trap -> " << ir; log() << m.str() << Symbols::LF; } }
            } else if (auto ers = dyn_cast<ErrorStmt>(st.get())) {
                // Evaluate error code and set globals
                std::string dv = emitExpr(out, ers->code.get(), "");
                std::string i32v = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", i32v, dv); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Error fptosi -> " << ir; log() << m.str() << Symbols::LF; } }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_err_code", i32v); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_err_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex); out << ir << Symbols::LF; }
                // Mirror into ERR/ERL variables
                ensureVarAllocated(out, "ERR");
                ensureVarAllocated(out, "ERL");
                std::string derr = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", derr, i32v); out << ir << Symbols::LF; }
                storeNumberToVar(out, "ERR", derr);
                std::string dln = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", dln, currentLine_); out << ir << Symbols::LF; }
                storeNumberToVar(out, "ERL", dln);
                // Branch to handler if installed; default to exit
                { std::string ir = std::format("  store i1 true, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
                std::string trap = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_err_trap_line", trap); out << ir << Symbols::LF; }
                { std::string ir = std::format("  switch i32 {}, label %exit [", trap); out << ir << Symbols::LF; }
                for (const auto & [lnum, lp] : lineMap_) {
                    (void)lp;
                    std::string ir = std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum));
                    out << ir << Symbols::LF;
                }
                out << "  ]" << Symbols::LF;
            } else if (auto rs = dyn_cast<ResumeStmt>(st.get())) {
                if (rs->kind == ResumeStmt::Kind::Line) {
                    { std::string ir = std::format("  store i1 false, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
                    std::string ir = std::format("  br label %{}", lineLabelName(rs->line));
                    out << ir << Symbols::LF;
                    { std::ostringstream m; m << "line " << currentLine_ << " Resume line -> " << ir; log() << m.str() << Symbols::LF; }
                    stmtTerminates = true;
                } else {
                    // Load resume line and dispatch: RESUME -> start of that line; RESUME NEXT -> next line.
                    // Use per-case trampoline blocks to clear handler flag before branching to the target.
                    std::string rln = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_resume_line", rln); out << ir << Symbols::LF; }
                    // Build switch mapping to case labels
                    std::string defLbl = lineLabelName(line.number) + std::string("_resume_def_") + std::to_string(++localContCounter);
                    { std::string ir = std::format("  switch i32 {}, label %{} [", rln, defLbl); out << ir << Symbols::LF; }
                    // We will materialize case blocks after the switch
                    struct ResumeCase { int line; std::string label; };
                    std::vector<ResumeCase> cases; cases.reserve(lineNumbers_.size());
                    for (const auto & [lnum, lp] : lineMap_) {
                        (void)lp;
                        std::string caseLbl = lineLabelName(line.number) + std::string("_resume_case_") + std::to_string(++localContCounter);
                        std::string ir = std::format("    i32 {}, label %{}", lnum, caseLbl);
                        out << ir << Symbols::LF;
                        cases.push_back({lnum, caseLbl});
                    }
                    out << "  ]" << Symbols::LF;
                    // Emit default: clear flag then exit
                    out << defLbl << ":" << Symbols::LF;
                    { std::string ir = std::format("  store i1 false, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF; }
                    // Emit each case trampoline: clear flag then branch to target
                    for (const auto& cs : cases) {
                        out << cs.label << ":" << Symbols::LF;
                        { std::string ir = std::format("  store i1 false, ptr @gwb_in_handler"); out << ir << Symbols::LF; }
                        std::string target;
                        if (rs->kind == ResumeStmt::Kind::Reexecute) {
                            target = lineLabelName(cs.line);
                        } else {
                            auto it = std::find(lineNumbers_.begin(), lineNumbers_.end(), cs.line);
                            if (it != lineNumbers_.end() && (it + 1) != lineNumbers_.end()) {
                                int nextLine = *(it + 1);
                                target = lineLabelName(nextLine);
                            } else {
                                target = "exit";
                            }
                        }
                        { std::string ir = std::format("  br label %{}", target); out << ir << Symbols::LF; }
                    }
                }
            } else if (auto op = dyn_cast<OpenStmt>(st.get())) {
                // OPEN <filename> FOR (INPUT|OUTPUT) AS #<channel>
                // Evaluate filename expression and call fopen with mode
                std::string fnptr = emitExpr(out, op->filename.get(), "");
                std::string mode = nextTemp();
                if (op->mode == FileMode::Input) {
                    { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.mode_r, i64 0", mode); out << ir << Symbols::LF; }
                } else {
                    { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.mode_w, i64 0", mode); out << ir << Symbols::LF; }
                }
                std::string f = nextTemp(); { std::string ir = std::format("  {} = call ptr @fopen(ptr {}, ptr {})", f, fnptr, mode); out << ir << Symbols::LF; }
                // Store file handle into channel table
                const int idx = op->channel - 1;
                std::string ep = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", ep, idx); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store ptr {}, ptr {}", f, ep); out << ir << Symbols::LF; }
            } else if (auto cl = dyn_cast<CloseStmt>(st.get())) {
                // CLOSE #<channel>
                const int idx = cl->channel - 1;
                std::string ep = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", ep, idx); out << ir << Symbols::LF; }
                std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, ep); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call i32 @fclose(ptr {})", fh); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store ptr null, ptr {}", ep); out << ir << Symbols::LF; }
            } else if (auto dim = dyn_cast<DimStmt>(st.get())) {
                // DIM is a no-op at runtime in this compiler
                { std::ostringstream m; m << "line " << currentLine_ << " DimStmt (no-op)"; log() << m.str() << Symbols::LF; }
            } else if (auto ds = dyn_cast<DataStmt>(st.get())) {
                // DATA: no runtime effect; items lowered into globals
                { std::ostringstream m; m << "line " << currentLine_ << " DataStmt (no-op)"; log() << m.str() << Symbols::LF; }
            } else if (auto rd = dyn_cast<ReadStmt>(st.get())) {
                // READ variables from @gwb_data
                for (const auto& t : rd->targets) {
                    // load index
                    std::string idx = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_data_idx", idx); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read load idx -> " << ir; log() << m.str() << Symbols::LF; } }
                    // compute element pointer
                    std::string idx64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", idx64, idx); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read sext -> " << ir; log() << m.str() << Symbols::LF; } }
                    std::string ep = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr @gwb_data, i64 0, i64 {}", ep, dataLiteralIds_.size(), idx64); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read gep -> " << ir; log() << m.str() << Symbols::LF; } }
                    std::string sval = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", sval, ep); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read load ptr -> " << ir; log() << m.str() << Symbols::LF; } }
                    // increment index
                    std::string idx1 = nextTemp(); { std::string ir = std::format("  {} = add i32 {}, 1", idx1, idx); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read add -> " << ir; log() << m.str() << Symbols::LF; } }
                    { std::string ir = std::format("  store i32 {}, ptr @gwb_data_idx", idx1); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read store idx -> " << ir; log() << m.str() << Symbols::LF; } }
                    // assign to target
                    if (t.index) {
                        // Array element numeric assignment
                        const int len = arraySizes_[t.name];
                        ensureArrayAllocated(out, t.name, len);
                        std::string base = arrayAllocaName_[t.name];
                        std::string idxReg = emitExpr(out, t.index.get(), "");
                        std::string i64i = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", i64i, idxReg); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read idx fptosi -> " << ir; log() << m.str() << Symbols::LF; } }
                        std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, i64i); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read arr gep -> " << ir; log() << m.str() << Symbols::LF; } }
                        std::string dval = nextTemp(); { std::string ir = std::format("  {} = call double @atof(ptr {})", dval, sval); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read atof -> " << ir; log() << m.str() << Symbols::LF; } }
                        { std::string ir = std::format("  store double {}, ptr {}", dval, elem); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read arr store -> " << ir; log() << m.str() << Symbols::LF; } }
                    } else {
                        // Scalar var
                        ensureVarAllocated(out, t.name);
                        if (isStringVarNameCG(t.name)) {
                            std::string ir = std::format("  store ptr {}, ptr {}", sval, varAllocaName_[t.name]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read store$ -> " << ir; log() << m.str() << Symbols::LF; }
                        } else {
                            std::string dval = nextTemp(); { std::string ir = std::format("  {} = call double @atof(ptr {})", dval, sval); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read atof -> " << ir; log() << m.str() << Symbols::LF; } }
                            std::string ir = std::format("  store double {}, ptr {}", dval, varAllocaName_[t.name]); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Read store -> " << ir; log() << m.str() << Symbols::LF; }
                        }
                    }
                }
            } else if (dyn_cast<MergeStmt>(st.get())) {
                { std::ostringstream m; m << "line " << currentLine_ << " MergeStmt (no-op)"; log() << m.str() << Symbols::LF; }
            } else if (isa<DefFnStmt>(st.get())) {
                // DEF FN has no direct runtime effect; handled via inlining in expressions.
                { std::ostringstream m; m << "line " << currentLine_ << " DefFnStmt (no-op)"; log() << m.str() << Symbols::LF; }
            } else if (isa<DefTypeStmt>(st.get())) {
                // DEFINT/DEFSNG/DEFDBL/DEFSTR are compile-time declarations; no direct codegen
                { std::ostringstream m; m << "line " << currentLine_ << " DefTypeStmt (no-op)"; log() << m.str() << Symbols::LF; }
            } else if (isa<DefSegStmt>(st.get())) {
                // DEF SEG is a no-op in this compiler
                { std::ostringstream m; m << "line " << currentLine_ << " DefSegStmt (no-op)"; log() << m.str() << Symbols::LF; }
            } else if (auto pk = dyn_cast<PokeStmt>(st.get())) {
                // Compute physical address = seg*16 + addr
                std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << Symbols::LF; }
                std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << Symbols::LF; }
                std::string off = emitExpr(out, pk->address.get(), "");
                std::string off64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off64, off); out << ir << Symbols::LF; }
                std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << Symbols::LF; }
                std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off64); out << ir << Symbols::LF; }
                std::string base = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", base, addr); out << ir << Symbols::LF; }
                std::string v = emitExpr(out, pk->value.get(), "");
                std::string vi = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", vi, v); out << ir << Symbols::LF; }
                std::string vb = nextTemp(); { std::string ir = std::format("  {} = trunc i32 {} to i8", vb, vi); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i8 {}, ptr {}", vb, base); out << ir << Symbols::LF; }
            } else if (auto bl = dyn_cast<BloadStmt>(st.get())) {
                // Open file for binary read; read into mem starting at seg*16 + offset
                auto fnptr = emitExpr(out, bl->filename.get(), "");
                std::string mode = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.mode_rb, i64 0", mode); out << ir << Symbols::LF; }
                std::string f = nextTemp(); { std::string ir = std::format("  {} = call ptr @fopen(ptr {}, ptr {})", f, fnptr, mode); out << ir << Symbols::LF; }
                std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << Symbols::LF; }
                std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << Symbols::LF; }
                std::string off = "0.0";
                if (bl->offset) off = emitExpr(out, bl->offset.get(), "");
                std::string off64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off64, off); out << ir << Symbols::LF; }
                std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << Symbols::LF; }
                std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off64); out << ir << Symbols::LF; }
                std::string dest = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", dest, addr); out << ir << Symbols::LF; }
                std::string remain = nextTemp(); { std::string ir = std::format("  {} = sub i64 1048576, {}", remain, addr); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call i64 @fread(ptr {}, i64 1, i64 {}, ptr {})", dest, remain, f); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call i32 @fclose(ptr {})", f); out << ir << Symbols::LF; }
            } else if (auto bs = dyn_cast<BsaveStmt>(st.get())) {
                auto fnptr = emitExpr(out, bs->filename.get(), "");
                std::string mode = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.mode_wb, i64 0", mode); out << ir << Symbols::LF; }
                std::string f = nextTemp(); { std::string ir = std::format("  {} = call ptr @fopen(ptr {}, ptr {})", f, fnptr, mode); out << ir << Symbols::LF; }
                std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << Symbols::LF; }
                std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << Symbols::LF; }
                std::string off = emitExpr(out, bs->offset.get(), "");
                std::string off64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off64, off); out << ir << Symbols::LF; }
                std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << Symbols::LF; }
                std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off64); out << ir << Symbols::LF; }
                std::string src = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", src, addr); out << ir << Symbols::LF; }
                std::string len = emitExpr(out, bs->length.get(), "");
                std::string len64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", len64, len); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call i64 @fwrite(ptr {}, i64 1, i64 {}, ptr {})", src, len64, f); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call i32 @fclose(ptr {})", f); out << ir << Symbols::LF; }
            } else if (auto ca = dyn_cast<CallAbsStmt>(st.get())) {
                // CALL: compute effective address (seg*16 + addr) and invoke helper
                std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL load seg -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL seg*16 -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string off = emitExpr(out, ca->address.get(), "");
                std::string off64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off64, off); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL addr fptosi -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL seg sext -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off64); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL eff addr -> " << ir; log() << m.str() << Symbols::LF; } }
                { std::string ir = std::format("  call void @gwb_call(i64 {})", addr); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL invoke -> " << ir; log() << m.str() << Symbols::LF; } }
            } else if (auto cd = dyn_cast<ChdirStmt>(st.get())) {
                std::string p = emitExpr(out, cd->path.get(), "");
                { std::string ir = std::format("  call i32 @chdir(ptr {})", p); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " ChdirStmt chdir -> " << ir; log() << m.str() << Symbols::LF; } }
            } else if (auto du = dyn_cast<DefUsrStmt>(st.get())) {
                // DEF USR is a no-op in this compiler (store ignored)
                { std::ostringstream m; m << "line " << currentLine_ << " DefUsrStmt (no-op)"; log() << m.str() << Symbols::LF; }
            } else if (auto col = dyn_cast<ColorStmt>(st.get())) {
                // Emit SGR codes via printf("%c[%dm", 27, code); no newline
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
            } else if (auto sc = dyn_cast<ScreenStmt>(st.get())) {
                // SCREEN [mode][,[cs][,[apage][,vpage]]]
                // For now, emit a call to a stub initializer with the mode (default 0)
                std::string modei32 = "0"; // as i32 literal by default
                if (sc->mode) {
                    std::string mv = emitExpr(out, sc->mode.get(), "");
                    std::string i32v = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", i32v, mv); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Screen fptosi -> " << ir; log() << m.str() << Symbols::LF; } }
                    modei32 = i32v;
                }
                { std::string ir = std::format("  call void @gwb_graphics_init(i32 {})", modei32); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Screen init -> " << ir; log() << m.str() << Symbols::LF; } }
            } else if (auto ci = dyn_cast<CircleStmt>(st.get())) {
                // Guard: skip graphics if not ready
                std::string rdy = nextTemp(); { std::string ir = std::format("  {} = load i1, ptr @gwb_gfx_ready", rdy); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Circle load gfx_ready -> " << ir; log() << m.str() << Symbols::LF; } }
                std::string doLbl = lineLabelName(line.number) + "_circle_do" + std::to_string(++localContCounter);
                std::string contLbl = lineLabelName(line.number) + "_circle_cont" + std::to_string(localContCounter);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", rdy, doLbl, contLbl); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Circle guard br -> " << ir; log() << m.str() << Symbols::LF; } }
                out << doLbl << ":" << Symbols::LF;
                std::string xv = emitExpr(out, ci->x.get(), "");
                std::string yv = emitExpr(out, ci->y.get(), "");
                std::string rv = emitExpr(out, ci->r.get(), "");
                std::string colori32 = "-1";
                if (ci->color) {
                    std::string cv = emitExpr(out, ci->color.get(), "");
                    std::string ci32 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", ci32, cv); out << ir << Symbols::LF; }
                    colori32 = ci32;
                }
                std::string sv = ci->start ? emitExpr(out, ci->start.get(), "") : "-1.0";
                std::string ev = ci->end ? emitExpr(out, ci->end.get(), "") : "-1.0";
                std::string av = ci->aspect ? emitExpr(out, ci->aspect.get(), "") : "-1.0";
                std::string stepv = ci->step ? "true" : "false";
                { std::string ir = std::format("  call void @gwb_gfx_circle(double {}, double {}, double {}, i32 {}, double {}, double {}, double {}, i1 {})", xv, yv, rv, colori32, sv, ev, av, stepv); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " Circle call -> " << ir; log() << m.str() << Symbols::LF; } }
                { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                out << contLbl << ":" << Symbols::LF;
            } else if (isa<ClearStmt>(st.get())) {
                // CLEAR: reset program state (scalars, strings, arrays in current segment), DATA index, file channels
                // Scope: limit to variables/arrays seen before this line to avoid crossing CHAIN boundaries
                const auto itVB = varsBeforeLine_.find(line.number);
                const std::set<std::string> emptyVars;
                const std::set<std::string>& vset = (itVB == varsBeforeLine_.end()) ? emptyVars : itVB->second;
                for (const auto &v: vset) {
                    auto it = varAllocaName_.find(v);
                    if (it == varAllocaName_.end()) continue;
                    resetVar(out, v);
                }
                // Arrays: zero elements for arrays seen before this line
                const auto itAB = arraysBeforeLine_.find(line.number);
                const std::set<std::string> emptyArr;
                const std::set<std::string>& aset = (itAB == arraysBeforeLine_.end()) ? emptyArr : itAB->second;
                for (const auto &an : aset) {
                    auto itLen = arraySizes_.find(an);
                    if (itLen == arraySizes_.end()) continue;
                    const int len = itLen->second;
                    ensureArrayAllocated(out, an, len);
                    std::string base = arrayAllocaName_[an];
                    for (int i = 0; i < len; ++i) {
                        std::string elem = nextTemp();
                        { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, i); out << ir << Symbols::LF; }
                        { std::string ir = std::format("  store double 0.0, ptr {}", elem); out << ir << Symbols::LF; }
                    }
                }
                // Reset DATA pointer to beginning
                { std::string ir = std::format("  store i32 0, ptr @gwb_data_idx"); out << ir << Symbols::LF; { std::ostringstream m; m << "line " << currentLine_ << " ClearStmt data_idx -> " << ir; log() << m.str() << Symbols::LF; } }
                // Close and clear file channel table entries
                for (int i = 0; i < 16; ++i) {
                    std::string ep = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", ep, i); out << ir << Symbols::LF; }
                    std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, ep); out << ir << Symbols::LF; }
                    std::string isnn = nextTemp(); { std::string ir = std::format("  {} = icmp ne ptr {}, null", isnn, fh); out << ir << Symbols::LF; }
                    std::string doLbl = lineLabelName(line.number) + "_clear_close_" + std::to_string(++localContCounter);
                    std::string contLbl = lineLabelName(line.number) + "_clear_cont_" + std::to_string(localContCounter);
                    { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isnn, doLbl, contLbl); out << ir << Symbols::LF; }
                    out << doLbl << ":" << Symbols::LF;
                    { std::string ir = std::format("  call i32 @fclose(ptr {})", fh); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  store ptr null, ptr {}", ep); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                    out << contLbl << ":" << Symbols::LF;
                }
            } else {
                throw CodeGenError("Unsupported statement encountered");
            }
            if (stmtTerminates) { terminated = true; break; }
        }
        if (!terminated) {
                std::string ir = std::format("  br label %{}", nextLabel);
                out << ir << Symbols::LF;
                { std::ostringstream m; m << "line " << currentLine_ << " fallthrough -> " << ir; log() << m.str() << Symbols::LF; }
        }
    }
} // namespace gwbasic
