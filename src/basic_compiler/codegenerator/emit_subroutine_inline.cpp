// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

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
    if (startIdx < 0) { out << entryLabel << ":\n"; out << "  br label %" << returnLabel << "\n"; return; }
    int localContCounter = 0;
    std::string currLabel = entryLabel;
    for (int idx = startIdx; idx < static_cast<int>(lineNumbers_.size()); ++idx) {
        int ln = lineNumbers_[idx];
        const Line* line = findLine(ln);
        if (!line) break;
        currentLine_ = ln;
        out << currLabel << ":\n";
        { std::ostringstream m; m << "begin subroutine line " << currentLine_; log(m.str()); }
        bool terminated = false;
        for (const auto& st : line->statements) {
            if (auto asg = dynamic_cast<AssignStmt*>(st.get())) {
                std::string val = emitExpr(out, asg->value.get(), entryLabel);
                std::string ir = "  store double "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
                out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " AssignStmt -> " << ir; log(m.str()); }
            } else if (auto pr = dynamic_cast<PrintStmt*>(st.get())) {
                if (dynamic_cast<StringExpr*>(pr->value.get())) {
                    int id = strLiteralId_[dynamic_cast<StringExpr*>(pr->value.get())->value];
                    std::string sptr = nextTemp();
                    std::string ir1 = "  "; ir1 += sptr; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += globalStringName(id); ir1 += ", i64 0";
                    out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir1; log(m.str()); }
                    std::string fmt = nextTemp();
                    std::string ir2 = "  "; ir2 += fmt; ir2 += " = getelementptr inbounds i8, ptr @.fmt_str, i64 0";
                    out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir2; log(m.str()); }
                    std::string ir3 = "  call i32 (ptr, ...) @printf(ptr "; ir3 += fmt; ir3 += ", ptr "; ir3 += sptr; ir3 += ")";
                    out << ir3 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir3; log(m.str()); }
                } else {
                    auto val = emitExpr(out, pr->value.get(), entryLabel);
                    std::string fmt = nextTemp();
                    std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr @.fmt_num, i64 0";
                    out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir1; log(m.str()); }
                    std::string ir2 = "  call i32 (ptr, ...) @printf(ptr "; ir2 += fmt; ir2 += ", double "; ir2 += val; ir2 += ")";
                    out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir2; log(m.str()); }
                }
            } else if (auto ins = dynamic_cast<InputStmt*>(st.get())) {
                std::string fmt = nextTemp();
                std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr @.fmt_in, i64 0";
                out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " InputStmt -> " << ir1; log(m.str()); }
                std::string ir2 = "  call i32 (ptr, ...) @scanf(ptr "; ir2 += fmt; ir2 += ", ptr "; ir2 += varAllocaName_[ins->name]; ir2 += ")";
                out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " InputStmt -> " << ir2; log(m.str()); }
            } else if (auto is = dynamic_cast<IfStmt*>(st.get())) {
                auto be = dynamic_cast<BinaryExpr*>(is->cond.get());
                if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt && be->op != BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) throw CodeGenError("IF condition must be a comparison");
                std::string cond = emitComparison(out, be);
                std::string contLbl = entryLabel; contLbl += "_cont"; contLbl += std::to_string(++localContCounter);
                std::string ir = "  br i1 "; ir += cond; ir += ", label %"; ir += lineLabelName(is->targetLine); ir += ", label %"; ir += contLbl;
                out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfStmt -> " << ir; log(m.str()); }
                out << contLbl << ":\n";
            } else if (auto gt = dynamic_cast<GotoStmt*>(st.get())) {
                std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine);
                out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " GotoStmt -> " << ir; log(m.str()); }
                terminated = true;
                break;
            } else if (auto gs = dynamic_cast<GosubStmt*>(st.get())) {
                std::string cont = entryLabel; cont += "_gosub_cont"; cont += std::to_string(++localContCounter);
                std::string ent = entryLabel; ent += "_gosub_entry"; ent += std::to_string(localContCounter);
                { std::string ir = "  br label %"; ir += ent; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " GosubStmt -> " << ir; log(m.str()); } }
                emitSubroutineInline(out, gs->targetLine, ent, cont);
                out << cont << ":\n";
            } else if (auto fs = dynamic_cast<ForStmt*>(st.get())) {
                emitFor(out, fs, entryLabel, localContCounter);
            } else if (dynamic_cast<ReturnStmt*>(st.get())) {
                std::string ir = "  br label %"; ir += returnLabel;
                out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ReturnStmt -> " << ir; log(m.str()); }
                terminated = true;
                break;
            } else if (dynamic_cast<EndStmt*>(st.get())) {
                std::string ir = "  br label %exit";
                out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " EndStmt -> " << ir; log(m.str()); }
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
            { std::string ir = "  br label %"; ir += currLabel; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " fallthrough -> " << ir; log(m.str()); } }
        } else { out << "  br label %" << returnLabel << "\n"; return; }
    }
}

} // namespace gwbasic
