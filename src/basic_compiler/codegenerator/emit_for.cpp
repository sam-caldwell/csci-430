// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitFor(std::ostringstream& out, const ForStmt* fs, const std::string& currLineLabel, int& localCounter) {
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
    std::string loopId = std::to_string(++localCounter);
    std::string condLbl = currLineLabel; condLbl += "_for_cond"; condLbl += loopId;
    std::string bodyLbl = currLineLabel; bodyLbl += "_for_body"; bodyLbl += loopId;
    std::string incLbl  = currLineLabel; incLbl  += "_for_inc";  incLbl  += loopId;
    std::string endLbl  = currLineLabel; endLbl  += "_for_end";  endLbl  += loopId;

    ensureVarAllocated(out, fs->var);
    {
        std::string startReg = emitExpr(out, fs->start.get(), currLineLabel);
        std::string ir1 = "  store double "; ir1 += startReg; ir1 += ", ptr "; ir1 += varAllocaName_[fs->var];
        out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt init -> " << ir1; log(m.str()); }
        std::string ir2 = "  br label %"; ir2 += condLbl;
        out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt -> " << ir2; log(m.str()); }
    }

    out << condLbl << ":\n";
    std::string curVal = nextTemp();
    {
        std::string ir = "  "; ir += curVal; ir += " = load double, ptr "; ir += varAllocaName_[fs->var];
        out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt cond load -> " << ir; log(m.str()); }
    }
    {
        std::string endReg = emitExpr(out, fs->end.get(), currLineLabel);
        std::string cond = nextTemp();
        std::string ir1 = "  "; ir1 += cond; ir1 += " = fcmp ole double "; ir1 += curVal; ir1 += ", "; ir1 += endReg;
        out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt cond cmp -> " << ir1; log(m.str()); }
        std::string ir2 = "  br i1 "; ir2 += cond; ir2 += ", label %"; ir2 += bodyLbl; ir2 += ", label %"; ir2 += endLbl;
        out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt branch -> " << ir2; log(m.str()); }
    }

    out << bodyLbl << ":\n";
    for (const auto& s : fs->body) {
        if (auto asg = dynamic_cast<AssignStmt*>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            std::string ir = "  store double "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt body Assign -> " << ir; log(m.str()); }
        } else if (auto pr = dynamic_cast<PrintStmt*>(s.get())) {
            if (dynamic_cast<StringExpr*>(pr->value.get())) {
                int id = strLiteralId_[dynamic_cast<StringExpr*>(pr->value.get())->value];
                std::string sptr = nextTemp();
              std::string ir1 = "  "; ir1 += sptr; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += globalStringName(id); ir1 += ", i64 0";
              out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt body Print -> " << ir1; log(m.str()); }
              std::string fmt = nextTemp();
              std::string ir2 = "  "; ir2 += fmt; ir2 += " = getelementptr inbounds i8, ptr @.fmt_str, i64 0";
              out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt body Print -> " << ir2; log(m.str()); }
              std::string ir3 = "  call i32 (ptr, ...) @printf(ptr "; ir3 += fmt; ir3 += ", ptr "; ir3 += sptr; ir3 += ")";
              out << ir3 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt body Print -> " << ir3; log(m.str()); }
            } else {
                auto val = emitExpr(out, pr->value.get(), currLineLabel);
              std::string fmt = nextTemp();
              std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr @.fmt_num, i64 0";
              out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt body Print -> " << ir1; log(m.str()); }
              std::string ir2 = "  call i32 (ptr, ...) @printf(ptr "; ir2 += fmt; ir2 += ", double "; ir2 += val; ir2 += ")";
              out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt body Print -> " << ir2; log(m.str()); }
            }
        } else {
            throw CodeGenError("Unsupported statement in FOR body");
        }
    }
    out << "  br label %" << incLbl << "\n";

    out << incLbl << ":\n";
    std::string stepReg = fs->step ? emitExpr(out, fs->step.get(), currLineLabel) : std::string("1.0");
    std::string vcur = nextTemp();
    { std::string ir = "  "; ir += vcur; ir += " = load double, ptr "; ir += varAllocaName_[fs->var]; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt inc load -> " << ir; log(m.str()); } }
    std::string vnext = nextTemp();
    { std::string ir = "  "; ir += vnext; ir += " = fadd double "; ir += vcur; ir += ", "; ir += stepReg; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt inc add -> " << ir; log(m.str()); } }
    { std::string ir = "  store double "; ir += vnext; ir += ", ptr "; ir += varAllocaName_[fs->var]; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt inc store -> " << ir; log(m.str()); } }
    { std::string ir = "  br label %"; ir += condLbl; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " ForStmt -> " << ir; log(m.str()); } }

    out << endLbl << ":\n";
}

} // namespace gwbasic
