// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include <sstream>
#include <format>

namespace gwbasic {

void CodeGenerator::emitSubroutineInline(std::ostringstream& out, int targetLine, const std::string& entryLabel, const std::string& returnLabel) {
    /*
     * Function: CodeGenerator::emitSubroutineInline
     * Inputs:
     *  - out: IR stream
     *  - targetLine: line number of subroutine entry
     *  - entryLabel/returnLabel: labels for entry/return points at call site
     * Outputs:
     *  - void
     * Theory of operation:
     *  - Walks lines starting at the target, emitting IR for each statement
     *    until encountering RETURN/END or running out of lines, threading
     *    through auto-generated continuation labels.
     */
    int startIdx = -1;
    for (size_t i = 0; i < lineNumbers_.size(); ++i) if (lineNumbers_[i] == targetLine) { startIdx = static_cast<int>(i); break; }
    if (startIdx < 0) { out << entryLabel << ":" << Symbols::LF; out << "  br label %" << returnLabel << Symbols::LF; return; }
    int localContCounter = 0;
    std::string currLabel = entryLabel;
    for (int idx = startIdx; idx < static_cast<int>(lineNumbers_.size()); ++idx) {
        int ln = lineNumbers_[idx];
        const Line* line = findLine(ln);
        if (!line) break;
        currentLine_ = ln;
        out << currLabel << ":" << Symbols::LF;
        log() << "begin subroutine line " << currentLine_ << Symbols::LF;
        bool terminated = false;
        for (const auto& st : line->statements) {
            if (auto asg = dyn_cast<AssignStmt>(st.get())) {
                std::string val = emitExpr(out, asg->value.get(), entryLabel);
                std::string ir;
            if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
                ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " AssignStmt -> " << ir << Symbols::LF;
            } else {
                storeNumberToVar(out, asg->name, val);
            }
            } else if (auto pr = dyn_cast<PrintStmt>(st.get())) {
                std::vector<const Expr*> items;
                if (pr->value) items.push_back(pr->value.get());
                for (const auto& v : pr->more) items.push_back(v.get());
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
                        auto sptr = emitExpr(out, v, entryLabel);
                        std::string fmt = nextTemp();
                        { std::string ir2 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " PrintStmt -> " << ir2 << Symbols::LF; }
                        { std::string ir3 = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, sptr); out << ir3 << Symbols::LF; log() << "line " << currentLine_ << " PrintStmt -> " << ir3 << Symbols::LF; }
                    } else {
                        auto val = emitExpr(out, v, entryLabel);
                        std::string fmt = nextTemp();
                        { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_num" : "@.fmt_num_sp")); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " PrintStmt -> " << ir1 << Symbols::LF; }
                        { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmt, val); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " PrintStmt -> " << ir2 << Symbols::LF; }
                    }
                }
            } else if (auto ins = dyn_cast<InputStmt>(st.get())) {
                std::string fmt = nextTemp();
                std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " InputStmt -> " << ir1 << Symbols::LF;
            // Read into a temp double, then convert to variable storage type
            std::string tmp = nextTemp(); { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
            std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp);
            out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InputStmt -> " << ir2 << Symbols::LF;
            std::string dv = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
            storeNumberToVar(out, ins->name, dv);
            } else if (auto is = dyn_cast<IfStmt>(st.get())) {
                auto be = dyn_cast<BinaryExpr>(is->cond.get());
                if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt && be->op != BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) throw CodeGenError("IF condition must be a comparison");
                std::string cond = emitComparison(out, be);
                std::string contLbl = entryLabel; contLbl += "_cont"; contLbl += std::to_string(++localContCounter);
                std::string ir = std::format("  br i1 {}, label %{}, label %{}", cond, lineLabelName(is->targetLine), contLbl);
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfStmt -> " << ir << Symbols::LF;
                out << contLbl << ":" << Symbols::LF;
            } else if (auto gt = dyn_cast<GotoStmt>(st.get())) {
                std::string ir = std::format("  br label %{}", lineLabelName(gt->targetLine));
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " GotoStmt -> " << ir << Symbols::LF;
                terminated = true;
                break;
            } else if (auto gs = dyn_cast<GosubStmt>(st.get())) {
                std::string cont = entryLabel; cont += "_gosub_cont"; cont += std::to_string(++localContCounter);
                std::string ent = entryLabel; ent += "_gosub_entry"; ent += std::to_string(localContCounter);
                { std::string ir = std::format("  br label %{}", ent); out << ir << Symbols::LF; log() << "line " << currentLine_ << " GosubStmt -> " << ir << Symbols::LF; }
                emitSubroutineInline(out, gs->targetLine, ent, cont);
                out << cont << ":" << Symbols::LF;
            } else if (auto fs = dyn_cast<ForStmt>(st.get())) {
                emitFor(out, fs, entryLabel, localContCounter);
            } else if (isa<ReturnStmt>(st.get())) {
                std::string ir = std::format("  br label %{}", returnLabel);
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " ReturnStmt -> " << ir << Symbols::LF;
                terminated = true;
                break;
            } else if (isa<EndStmt>(st.get())) {
                std::string ir = std::format("  br label %exit");
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " EndStmt -> " << ir << Symbols::LF;
                terminated = true;
                break;
            } else {
                throw CodeGenError("Unsupported statement in GOSUB body");
            }
        }
        if (terminated) return;
        if (idx + 1 < static_cast<int>(lineNumbers_.size())) {
            {
                std::string label = entryLabel; label += "_n"; label += std::to_string(idx - startIdx + 1);
                currLabel = std::move(label);
            }
            { std::string ir = std::format("  br label %{}", currLabel); out << ir << Symbols::LF; log() << "line " << currentLine_ << " fallthrough -> " << ir << Symbols::LF; }
        } else { out << std::format("  br label %{}", returnLabel) << Symbols::LF; return; }
    }
}

} // namespace gwbasic
