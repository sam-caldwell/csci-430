// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
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
                if (!asg->name.empty() && asg->name.back() == CH_DOLLARSIGN) {
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
                    auto isStr = [&](const Expr* e, const auto& self) -> bool {
                        if (isa<StringExpr>(e)) return true;
                        if (auto vv = dyn_cast<VarExpr>(e)) return !vv->name.empty() && vv->name.back() == CH_DOLLARSIGN;
                        if (auto bb = dyn_cast<BinaryExpr>(e)) return (bb->op == BinaryOp::Add) && (self(bb->lhs.get(), self) || self(bb->rhs.get(), self));
                        return false;
                    };
                    if (isStr(v, isStr)) {
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
                        if (!t.name.empty() && t.name.back() == CH_DOLLARSIGN) {
                            std::string ir = std::format("  store ptr {}, ptr {}", sval, varAllocaName_[t.name]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read store$ -> " << ir; log() << m.str() << CH_LF; }
                        } else {
                            std::string dval = nextTemp(); { std::string ir = std::format("  {} = call double @atof(ptr {})", dval, sval); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read atof -> " << ir; log() << m.str() << CH_LF; } }
                            std::string ir = std::format("  store double {}, ptr {}", dval, varAllocaName_[t.name]); out << ir << STR_LF; { std::ostringstream m; m << "line " << currentLine_ << " Read store -> " << ir; log() << m.str() << CH_LF; }
                        }
                    }
                }
            } else if (dyn_cast<MergeStmt>(st.get())) {
                { std::ostringstream m; m << "line " << currentLine_ << " MergeStmt (no-op)"; log() << m.str() << CH_LF; }
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
