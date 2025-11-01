// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitLineBlock(std::ostringstream& out, const Line& line, int lineIndex, int lastIndex) {
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
    out << lineLabelName(line.number) << ":\n";
    {
        std::ostringstream m; m << "begin line " << currentLine_;
        log(m.str());
    }
    int localContCounter = 0;
    auto nextLabel = (lineIndex < lastIndex) ? lineLabelName(lineNumbers_[lineIndex + 1]) : std::string("exit");
    bool terminated = false;
    for (size_t i = 0; i < line.statements.size(); ++i) {
        const auto& st = line.statements[i];
        if (auto asg = dyn_cast<AssignStmt>(st.get())) {
            std::string val = emitExpr(out, asg->value.get(), "");
            std::string ir = "  store double "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
            out << ir << "\n";
            std::ostringstream m; m << "line " << currentLine_ << ' ' << nodeName(st.get()) << " -> " << ir; log(m.str());
        } else if (auto pr = dyn_cast<PrintStmt>(st.get())) {
            if (isa<StringExpr>(pr->value.get())) {
                int id = strLiteralId_[dyn_cast<StringExpr>(pr->value.get())->value];
                std::string sptr = nextTemp();
                std::string ir1 = "  "; ir1 += sptr; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += globalStringName(id); ir1 += ", i64 0";
                out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir1; log(m.str()); }
                std::string fmt = nextTemp();
                std::string ir2 = "  "; ir2 += fmt; ir2 += " = getelementptr inbounds i8, ptr @.fmt_str, i64 0";
                out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir2; log(m.str()); }
                std::string ir3 = "  call i32 (ptr, ...) @printf(ptr "; ir3 += fmt; ir3 += ", ptr "; ir3 += sptr; ir3 += ")";
                out << ir3 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir3; log(m.str()); }
            } else {
                auto val = emitExpr(out, pr->value.get(), "");
                std::string fmt = nextTemp();
                std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr @.fmt_num, i64 0";
                out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir1; log(m.str()); }
                std::string ir2 = "  call i32 (ptr, ...) @printf(ptr "; ir2 += fmt; ir2 += ", double "; ir2 += val; ir2 += ")";
                out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " PrintStmt -> " << ir2; log(m.str()); }
            }
        } else if (auto gt = dyn_cast<GotoStmt>(st.get())) {
            std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine);
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " GotoStmt -> " << ir; log(m.str()); }
            terminated = true;
            break;
        } else if (auto gs = dyn_cast<GosubStmt>(st.get())) {
            std::string contLbl = lineLabelName(line.number); contLbl += "_gosub_cont"; contLbl += std::to_string(++localContCounter);
            std::string entryLbl = lineLabelName(line.number); entryLbl += "_gosub_entry"; entryLbl += std::to_string(localContCounter);
            out << "  br label %" << entryLbl << "\n";
            emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
            out << contLbl << ":\n";
        } else if (auto is = dyn_cast<IfStmt>(st.get())) {
            auto be = dyn_cast<BinaryExpr>(is->cond.get());
            if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt && be->op != BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) {
                throw CodeGenError("IF condition must be a comparison");
            }
            std::string cond = emitComparison(out, be);
            std::string contLbl = "line"; contLbl += std::to_string(line.number); contLbl += "_cont"; contLbl += std::to_string(++localContCounter);
            std::string ir = "  br i1 "; ir += cond; ir += ", label %"; ir += lineLabelName(is->targetLine); ir += ", label %"; ir += contLbl;
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfStmt -> " << ir; log(m.str()); }
            out << contLbl << ":\n";
        } else if (isa<EndStmt>(st.get())) {
            std::string ir = "  br label %exit";
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " EndStmt -> " << ir; log(m.str()); }
            terminated = true;
            break;
        } else if (auto ins = dyn_cast<InputStmt>(st.get())) {
            ensureVarAllocated(out, ins->name);
            std::string fmt = nextTemp();
            std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr @.fmt_in, i64 0";
            out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " InputStmt -> " << ir1; log(m.str()); }
            std::string ir2 = "  call i32 (ptr, ...) @scanf(ptr "; ir2 += fmt; ir2 += ", ptr "; ir2 += varAllocaName_[ins->name]; ir2 += ")";
            out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " InputStmt -> " << ir2; log(m.str()); }
        } else if (auto fs = dyn_cast<ForStmt>(st.get())) {
            emitFor(out, fs, lineLabelName(line.number), localContCounter);
        } else if (isa<ReturnStmt>(st.get())) {
            std::string ir = "  br label %exit";
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ReturnStmt -> " << ir; log(m.str()); }
            terminated = true;
            break;
        } else {
            throw CodeGenError("Unsupported statement encountered");
        }
    }
    if (!terminated) { std::string ir = "  br label %"; ir += nextLabel; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " fallthrough -> " << ir; log(m.str()); } }
}

} // namespace gwbasic
