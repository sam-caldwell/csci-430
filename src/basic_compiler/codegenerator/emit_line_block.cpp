// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/DimStmt.h"
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
#include <sstream>
#include <format>

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
        out << lineLabelName(line.number) << ":" << STR_LF;
        { std::ostringstream m; m << "begin line " << currentLine_; log() << m.str() << CH_LF; }
        int localContCounter = 0;
        auto nextLabel = (lineIndex < lastIndex) ? lineLabelName(lineNumbers_[lineIndex + 1]) : std::string("exit");
        bool terminated = false;
        for (size_t i = 0; i < line.statements.size(); ++i) {
            const auto &st = line.statements[i];
            if (auto asg = dyn_cast<AssignStmt>(st.get())) {
                std::string val = emitExpr(out, asg->value.get(), "");
                std::string ir;
                if (isStringVarNameCG(asg->name)) {
                    ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
                } else {
                    ir = std::format("  store double {}, ptr {}", val, varAllocaName_[asg->name]);
                }
                out << ir << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << ' ' << nodeName(st.get()) << " -> " << ir; log() << m.str() << CH_LF; }
            } else if (auto aaset = dyn_cast<ArrayAssignStmt>(st.get())) {
                // A(i) = expr
                const int len = arraySizes_[aaset->name];
                ensureArrayAllocated(out, aaset->name, len);
                std::string base = arrayAllocaName_[aaset->name];
                std::string idxReg = emitExpr(out, aaset->index.get(), "");
                std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Array idx -> " << ir; log() << m.str() << CH_LF; } }
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Array gep -> " << ir; log() << m.str() << CH_LF; } }
                std::string val = emitExpr(out, aaset->value.get(), "");
                { std::string ir = std::format("  store double {}, ptr {}", val, elem); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Array store -> " << ir; log() << m.str() << CH_LF; } }
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
                        { std::string ir2 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")); out << ir2 << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir2; log() << m.str() << CH_LF; } }
                        // Optional USING override
                        std::string useFmt = fmt;
                        if (pr->format) { useFmt = emitExpr(out, pr->format.get(), ""); }
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << STR_LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << STR_LF; }
                            { std::string ir3 = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fh, useFmt, sptr); out << ir3 << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt fprintf -> " << ir3; log() << m.str() << CH_LF; } }
                        } else {
                            { std::string ir3 = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", useFmt, sptr); out << ir3 << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt printf -> " << ir3; log() << m.str() << CH_LF; } }
                            // Mirror to virtual screen using snprintf and helper
                            std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << STR_LF; }
                            std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, useFmt, sptr); out << irn << STR_LF; }
                            std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << STR_LF; }
                            { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << STR_LF; }
                        }
                    } else {
                        auto val = emitExpr(out, v, "");
                        std::string fmt = nextTemp();
                        {
                            std::string ir1 = "  ";
                            ir1 += fmt;
                            ir1 += " = getelementptr inbounds i8, ptr ";
                            ir1 += (last ? "@.fmt_num" : "@.fmt_num_sp");
                            ir1 += ", i64 0";
                            out << ir1 << STR_LF;
                            {
                                std::ostringstream m;
                                m << "line " << currentLine_ << " PrintStmt -> " << ir1;
                            log() << m.str() << CH_LF;
                            }
                        }
                        // Optional USING override
                        std::string useFmt2 = fmt;
                        if (pr->format) { useFmt2 = emitExpr(out, pr->format.get(), ""); }
                        if (pr->channel >= 1) {
                            std::string fptr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1); out << ir << STR_LF; }
                            std::string fh = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", fh, fptr); out << ir << STR_LF; }
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, useFmt2, val); out << ir2 << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt fprintf -> " << ir2; log() << m.str() << CH_LF; } }
                        } else {
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", useFmt2, val); out << ir2 << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt printf -> " << ir2; log() << m.str() << CH_LF; } }
                            // Mirror to virtual screen using snprintf and helper
                            std::string sbuf = nextTemp(); { std::string irb = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << irb << STR_LF; }
                            std::string n = nextTemp(); { std::string irn = std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})", n, sbuf, useFmt2, val); out << irn << STR_LF; }
                            std::string n64 = nextTemp(); { std::string irl = std::format("  {} = sext i32 {} to i64", n64, n); out << irl << STR_LF; }
                            { std::string irw = std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64); out << irw << STR_LF; }
                        }
                    }
                }
            } else if (auto gt = dyn_cast<GotoStmt>(st.get())) {
                std::string ir = std::format("  br label %{}", lineLabelName(gt->targetLine));
                out << ir << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << " GotoStmt -> " << ir; log() << m.str() << CH_LF; }
                terminated = true;
                break;
            } else if (auto gs = dyn_cast<GosubStmt>(st.get())) {
                std::string contLbl = lineLabelName(line.number);
                contLbl += "_gosub_cont";
                contLbl += std::to_string(++localContCounter);
                std::string entryLbl = lineLabelName(line.number);
                entryLbl += "_gosub_entry";
                entryLbl += std::to_string(localContCounter);
                out << "  br label %" << entryLbl << STR_LF;
                emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
                out << contLbl << ":" << STR_LF;
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
                out << ir << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << " IfStmt -> " << ir; log() << m.str() << CH_LF; }
                out << contLbl << ":" << STR_LF;
            } else if (auto ib = dyn_cast<IfBlockStmt>(st.get())) {
                emitIfBlock(out, ib, lineLabelName(line.number), localContCounter);
            } else if (auto ws = dyn_cast<WhileStmt>(st.get())) {
                emitWhile(out, ws, lineLabelName(line.number), localContCounter);
            } else if (isa<EndStmt>(st.get())) {
                std::string ir = std::format("  br label %exit");
                out << ir << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << " EndStmt -> " << ir; log() << m.str() << CH_LF; }
                terminated = true;
                break;
            } else if (auto ins = dyn_cast<InputStmt>(st.get())) {
                ensureVarAllocated(out, ins->name);
                std::string fmt = nextTemp();
                std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt);
                out << ir1 << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << " InputStmt -> " << ir1; log() << m.str() << CH_LF; }
                std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, varAllocaName_[ins->name]);
                out << ir2 << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << " InputStmt -> " << ir2; log() << m.str() << CH_LF; }
            } else if (auto fs = dyn_cast<ForStmt>(st.get())) {
                emitFor(out, fs, lineLabelName(line.number), localContCounter);
            } else if (auto rz = dyn_cast<RandomizeStmt>(st.get())) {
                // RANDOMIZE [expr]
                if (rz->seed) {
                    auto val = emitExpr(out, rz->seed.get(), "");
                    std::string si = nextTemp();
                    {
                        std::string ir = std::format("  {} = fptosi double {} to i64", si, val);
                        out << ir << STR_LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " Randomize fptosi -> " << ir; log() << m.str() << CH_LF; }
                    }
                    {
                        std::string ir = std::format("  call void @srand48(i64 {})", si);
                        out << ir << STR_LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " Randomize srand48 -> " << ir; log() << m.str() << CH_LF; }
                    }
                } else {
                    std::string t = nextTemp();
                    {
                        std::string ir = std::format("  {} = call i64 @time(ptr null)", t);
                        out << ir << STR_LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " Randomize time -> " << ir; log() << m.str() << CH_LF; }
                    }
                    {
                        std::string ir = std::format("  call void @srand48(i64 {})", t);
                        out << ir << STR_LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " Randomize srand48(time) -> " << ir; log() << m.str() << CH_LF; }
                    }
                }
            } else if (isa<ReturnStmt>(st.get())) {
                std::string ir = std::format("  br label %exit");
                out << ir << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << " ReturnStmt -> " << ir; log() << m.str() << CH_LF; }
                terminated = true;
                break;
            } else if (auto cs = dyn_cast<CommonStmt>(st.get())) {
                // COMMON has no direct codegen effect in this compiler; treat as no-op.
                { std::ostringstream m; m << "line " << currentLine_ << " CommonStmt (no-op)"; log() << m.str() << CH_LF; }
            } else if (auto rn = dyn_cast<RunStmt>(st.get())) {
                // Reset all variables to 0.0 and branch to first or specified line
                for (const auto &v: variables_) {
                    auto it = varAllocaName_.find(v);
                    if (it == varAllocaName_.end()) continue;
                    std::string ir = std::format("  store double 0.0, ptr {}", it->second);
                    out << ir << STR_LF;
                    { std::ostringstream m; m << "line " << currentLine_ << " RunStmt reset -> " << ir; log() << m.str() << CH_LF; }
                }
                int dest = rn->targetLine.has_value()
                               ? *rn->targetLine
                               : (lineNumbers_.empty() ? line.number : lineNumbers_.front());
                std::string ir = "  br label %";
                ir += lineLabelName(dest);
                out << ir << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << " RunStmt branch -> " << ir; log() << m.str() << CH_LF; }
                terminated = true;
                break;
            } else if (auto ch = dyn_cast<ChainStmt>(st.get())) {
                // CHAIN: reset non-preserved variables and branch to target/first line
                if (!ch->all) {
                    // Preserve only variables declared COMMON before this line
                    const auto itCBL = commonBeforeLine_.find(line.number);
                    const std::set<std::string> emptySet;
                    const std::set<std::string>& preserve = (itCBL == commonBeforeLine_.end()) ? emptySet : itCBL->second;
                    for (const auto &v: variables_) {
                        if (preserve.contains(v)) continue; // preserve caller's COMMON only
                        auto it = varAllocaName_.find(v);
                        if (it == varAllocaName_.end()) continue;
                        std::string ir = "  store double 0.0, ptr ";
                        ir += it->second;
                        out << ir << STR_LF;
                        { std::ostringstream m; m << "line " << currentLine_ << " ChainStmt reset -> " << ir; log() << m.str() << CH_LF; }
                    }
                }
                int dest = ch->targetLine.has_value()
                               ? *ch->targetLine
                               : (lineNumbers_.empty() ? line.number : lineNumbers_.front());
                std::string ir = "  br label %";
                ir += lineLabelName(dest);
                out << ir << STR_LF;
                { std::ostringstream m; m << "line " << currentLine_ << " ChainStmt branch -> " << ir; log() << m.str() << CH_LF; }
                terminated = true;
                break;
            } else if (auto dim = dyn_cast<DimStmt>(st.get())) {
                // DIM is a no-op at runtime in this compiler
                { std::ostringstream m; m << "line " << currentLine_ << " DimStmt (no-op)"; log() << m.str() << CH_LF; }
            } else if (auto ds = dyn_cast<DataStmt>(st.get())) {
                // DATA: no runtime effect; items lowered into globals
                { std::ostringstream m; m << "line " << currentLine_ << " DataStmt (no-op)"; log() << m.str() << CH_LF; }
            } else if (auto rd = dyn_cast<ReadStmt>(st.get())) {
                // READ variables from @gwb_data
                for (const auto& t : rd->targets) {
                    // load index
                    std::string idx = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr @gwb_data_idx", idx); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read load idx -> " << ir; log() << m.str() << CH_LF; } }
                    // compute element pointer
                    std::string idx64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", idx64, idx); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read sext -> " << ir; log() << m.str() << CH_LF; } }
                    std::string ep = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr @gwb_data, i64 0, i64 {}", ep, dataLiteralIds_.size(), idx64); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read gep -> " << ir; log() << m.str() << CH_LF; } }
                    std::string sval = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", sval, ep); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read load ptr -> " << ir; log() << m.str() << CH_LF; } }
                    // increment index
                    std::string idx1 = nextTemp(); { std::string ir = std::format("  {} = add i32 {}, 1", idx1, idx); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read add -> " << ir; log() << m.str() << CH_LF; } }
                    { std::string ir = std::format("  store i32 {}, ptr @gwb_data_idx", idx1); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read store idx -> " << ir; log() << m.str() << CH_LF; } }
                    // assign to target
                    if (t.index) {
                        // Array element numeric assignment
                        const int len = arraySizes_[t.name];
                        ensureArrayAllocated(out, t.name, len);
                        std::string base = arrayAllocaName_[t.name];
                        std::string idxReg = emitExpr(out, t.index.get(), "");
                        std::string i64i = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", i64i, idxReg); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read idx fptosi -> " << ir; log() << m.str() << CH_LF; } }
                        std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, i64i); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read arr gep -> " << ir; log() << m.str() << CH_LF; } }
                        std::string dval = nextTemp(); { std::string ir = std::format("  {} = call double @atof(ptr {})", dval, sval); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read atof -> " << ir; log() << m.str() << CH_LF; } }
                        { std::string ir = std::format("  store double {}, ptr {}", dval, elem); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read arr store -> " << ir; log() << m.str() << CH_LF; } }
                    } else {
                        // Scalar var
                        ensureVarAllocated(out, t.name);
                        if (isStringVarNameCG(t.name)) {
                            std::string ir = std::format("  store ptr {}, ptr {}", sval, varAllocaName_[t.name]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read store$ -> " << ir; log() << m.str() << CH_LF; }
                        } else {
                            std::string dval = nextTemp(); { std::string ir = std::format("  {} = call double @atof(ptr {})", dval, sval); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read atof -> " << ir; log() << m.str() << CH_LF; } }
                            std::string ir = std::format("  store double {}, ptr {}", dval, varAllocaName_[t.name]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read store -> " << ir; log() << m.str() << CH_LF; }
                        }
                    }
                }
            } else if (dyn_cast<MergeStmt>(st.get())) {
                { std::ostringstream m; m << "line " << currentLine_ << " MergeStmt (no-op)"; log() << m.str() << CH_LF; }
            } else if (isa<DefFnStmt>(st.get())) {
                // DEF FN has no direct runtime effect; handled via inlining in expressions.
                { std::ostringstream m; m << "line " << currentLine_ << " DefFnStmt (no-op)"; log() << m.str() << CH_LF; }
            } else if (isa<DefTypeStmt>(st.get())) {
                // DEFINT/DEFSNG/DEFDBL/DEFSTR are compile-time declarations; no direct codegen
                { std::ostringstream m; m << "line " << currentLine_ << " DefTypeStmt (no-op)"; log() << m.str() << CH_LF; }
            } else if (isa<DefSegStmt>(st.get())) {
                // DEF SEG is a no-op in this compiler
                { std::ostringstream m; m << "line " << currentLine_ << " DefSegStmt (no-op)"; log() << m.str() << CH_LF; }
            } else if (auto pk = dyn_cast<PokeStmt>(st.get())) {
                // Compute physical address = seg*16 + addr
                std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << STR_LF; }
                std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << STR_LF; }
                std::string off = emitExpr(out, pk->address.get(), "");
                std::string off64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off64, off); out << ir << STR_LF; }
                std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << STR_LF; }
                std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off64); out << ir << STR_LF; }
                std::string base = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", base, addr); out << ir << STR_LF; }
                std::string v = emitExpr(out, pk->value.get(), "");
                std::string vi = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", vi, v); out << ir << STR_LF; }
                std::string vb = nextTemp(); { std::string ir = std::format("  {} = trunc i32 {} to i8", vb, vi); out << ir << STR_LF; }
                { std::string ir = std::format("  store i8 {}, ptr {}", vb, base); out << ir << STR_LF; }
            } else if (auto bl = dyn_cast<BloadStmt>(st.get())) {
                // Open file for binary read; read into mem starting at seg*16 + offset
                auto fnptr = emitExpr(out, bl->filename.get(), "");
                std::string mode = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.mode_rb, i64 0", mode); out << ir << STR_LF; }
                std::string f = nextTemp(); { std::string ir = std::format("  {} = call ptr @fopen(ptr {}, ptr {})", f, fnptr, mode); out << ir << STR_LF; }
                std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << STR_LF; }
                std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << STR_LF; }
                std::string off = "0.0";
                if (bl->offset) off = emitExpr(out, bl->offset.get(), "");
                std::string off64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off64, off); out << ir << STR_LF; }
                std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << STR_LF; }
                std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off64); out << ir << STR_LF; }
                std::string dest = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", dest, addr); out << ir << STR_LF; }
                std::string remain = nextTemp(); { std::string ir = std::format("  {} = sub i64 1048576, {}", remain, addr); out << ir << STR_LF; }
                { std::string ir = std::format("  call i64 @fread(ptr {}, i64 1, i64 {}, ptr {})", dest, remain, f); out << ir << STR_LF; }
                { std::string ir = std::format("  call i32 @fclose(ptr {})", f); out << ir << STR_LF; }
            } else if (auto bs = dyn_cast<BsaveStmt>(st.get())) {
                auto fnptr = emitExpr(out, bs->filename.get(), "");
                std::string mode = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.mode_wb, i64 0", mode); out << ir << STR_LF; }
                std::string f = nextTemp(); { std::string ir = std::format("  {} = call ptr @fopen(ptr {}, ptr {})", f, fnptr, mode); out << ir << STR_LF; }
                std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << STR_LF; }
                std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << STR_LF; }
                std::string off = emitExpr(out, bs->offset.get(), "");
                std::string off64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off64, off); out << ir << STR_LF; }
                std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << STR_LF; }
                std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off64); out << ir << STR_LF; }
                std::string src = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", src, addr); out << ir << STR_LF; }
                std::string len = emitExpr(out, bs->length.get(), "");
                std::string len64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", len64, len); out << ir << STR_LF; }
                { std::string ir = std::format("  call i64 @fwrite(ptr {}, i64 1, i64 {}, ptr {})", src, len64, f); out << ir << STR_LF; }
                { std::string ir = std::format("  call i32 @fclose(ptr {})", f); out << ir << STR_LF; }
            } else if (auto ca = dyn_cast<CallAbsStmt>(st.get())) {
                // CALL: compute effective address (seg*16 + addr) and invoke helper
                std::string seg = nextTemp(); { std::string ir = "  "; ir += seg; ir += " = load i32, ptr @gwb_seg"; out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL load seg -> " << ir; log() << m.str() << CH_LF; } }
                std::string seg16 = nextTemp(); { std::string ir = std::format("  {} = mul i32 {}, 16", seg16, seg); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL seg*16 -> " << ir; log() << m.str() << CH_LF; } }
                std::string off = emitExpr(out, ca->address.get(), "");
                std::string off64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", off64, off); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL addr fptosi -> " << ir; log() << m.str() << CH_LF; } }
                std::string seg64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", seg64, seg16); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL seg sext -> " << ir; log() << m.str() << CH_LF; } }
                std::string addr = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", addr, seg64, off64); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL eff addr -> " << ir; log() << m.str() << CH_LF; } }
                { std::string ir = std::format("  call void @gwb_call(i64 {})", addr); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " CALL invoke -> " << ir; log() << m.str() << CH_LF; } }
            } else if (auto cd = dyn_cast<ChdirStmt>(st.get())) {
                std::string p = emitExpr(out, cd->path.get(), "");
                { std::string ir = std::format("  call i32 @chdir(ptr {})", p); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " ChdirStmt chdir -> " << ir; log() << m.str() << CH_LF; } }
            } else if (auto du = dyn_cast<DefUsrStmt>(st.get())) {
                // DEF USR is a no-op in this compiler (store ignored)
                { std::ostringstream m; m << "line " << currentLine_ << " DefUsrStmt (no-op)"; log() << m.str() << CH_LF; }
            } else if (auto col = dyn_cast<ColorStmt>(st.get())) {
                // Emit SGR codes via printf("%c[%dm", 27, code); no newline
                auto emitColor = [&](const std::unique_ptr<Expr>& e, bool isFg){
                    if (!e) return;
                    std::string val = emitExpr(out, e.get(), "");
                    std::string i32v = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", i32v, val); out << ir << STR_LF; }
                    std::string masked = nextTemp(); { std::string ir = std::format("  {} = and i32 {}, 15", masked, i32v); out << ir << STR_LF; }
                    std::string idx64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", idx64, masked); out << ir << STR_LF; }
                    std::string p = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x i32], ptr {}, i64 0, i64 {}", p, (isFg?"@.sgr_fg_tbl":"@.sgr_bg_tbl"), idx64); out << ir << STR_LF; }
                    std::string code = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr {}", code, p); out << ir << STR_LF; }
                    std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [7 x i8], ptr @.fmt_sgr, i64 0, i64 0", fmt); out << ir << STR_LF; }
                    { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 27, i32 {})", fmt, code); out << ir << STR_LF; }
                };
                emitColor(col->fg, true);
                emitColor(col->bg, false);
            } else if (auto sc = dyn_cast<ScreenStmt>(st.get())) {
                // SCREEN [mode][,[cs][,[apage][,vpage]]]
                // For now, emit a call to a stub initializer with the mode (default 0)
                std::string modei32 = "0"; // as i32 literal by default
                if (sc->mode) {
                    std::string mv = emitExpr(out, sc->mode.get(), "");
                    std::string i32v = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", i32v, mv); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Screen fptosi -> " << ir; log() << m.str() << CH_LF; } }
                    modei32 = i32v;
                }
                { std::string ir = std::format("  call void @gwb_graphics_init(i32 {})", modei32); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Screen init -> " << ir; log() << m.str() << CH_LF; } }
            } else {
                throw CodeGenError("Unsupported statement encountered");
            }
        }
        if (!terminated) {
            std::string ir = std::format("  br label %{}", nextLabel);
            out << ir << STR_LF;
            { std::ostringstream m; m << "line " << currentLine_ << " fallthrough -> " << ir; log() << m.str() << CH_LF; }
        }
    }
} // namespace gwbasic
