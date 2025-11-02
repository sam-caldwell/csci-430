// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitWhile(std::ostringstream& out, const WhileStmt* ws, const std::string& currLineLabel, int& localCounter) {
    /*
     * Emit single-line WHILE loop:
     *  cond -> body -> jump cond -> end
     */
    std::string id = std::to_string(++localCounter);
    std::string condLbl = currLineLabel; condLbl += "_while_cond"; condLbl += id;
    std::string bodyLbl = currLineLabel; bodyLbl += "_while_body"; bodyLbl += id;
    std::string endLbl  = currLineLabel; endLbl  += "_while_end";  endLbl  += id;

    // Jump to condition
    { std::string ir = "  br label %"; ir += condLbl; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While -> " << ir; log(m.str()); } }

    // Condition
    out << condLbl << ":\n";
    auto be = dyn_cast<const BinaryExpr>(ws->cond.get());
    if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt && be->op != BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) {
        throw CodeGenError("WHILE condition must be a comparison");
    }
    std::string cond = emitComparison(out, be);
    { std::string ir = "  br i1 "; ir += cond; ir += ", label %"; ir += bodyLbl; ir += ", label %"; ir += endLbl; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While branch -> " << ir; log(m.str()); } }

    // Body
    out << bodyLbl << ":\n";
    for (const auto& s : ws->body) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            std::string ir = "  store double "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Assign -> " << ir; log(m.str()); }
        } else if (auto pr = dyn_cast<PrintStmt>(s.get())) {
            std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
            for (size_t pi = 0; pi < items.size(); ++pi) {
                const bool last = (pi + 1 == items.size());
                const Expr* v = items[pi];
                if (isa<StringExpr>(v)) {
                    int id2 = strLiteralId_[dyn_cast<StringExpr>(v)->value];
                    std::string sptr = nextTemp(); { std::string ir1 = "  "; ir1 += sptr; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += globalStringName(id2); ir1 += ", i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Print -> " << ir1; log(m.str()); } }
                    std::string fmt = nextTemp(); { std::string ir2 = "  "; ir2 += fmt; ir2 += " = getelementptr inbounds i8, ptr "; ir2 += (last ? "@.fmt_str" : "@.fmt_str_sp"); ir2 += ", i64 0"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Print -> " << ir2; log(m.str()); } }
                    { std::string ir3 = "  call i32 (ptr, ...) @printf(ptr "; ir3 += fmt; ir3 += ", ptr "; ir3 += sptr; ir3 += ")"; out << ir3 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Print -> " << ir3; log(m.str()); } }
                } else {
                    auto val = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp(); { std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += (last ? "@.fmt_num" : "@.fmt_num_sp"); ir1 += ", i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Print -> " << ir1; log(m.str()); } }
                    { std::string ir2 = "  call i32 (ptr, ...) @printf(ptr "; ir2 += fmt; ir2 += ", double "; ir2 += val; ir2 += ")"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Print -> " << ir2; log(m.str()); } }
                }
            }
        } else if (auto fs = dyn_cast<ForStmt>(s.get())) {
            emitFor(out, fs, currLineLabel, localCounter);
        } else if (auto ib = dyn_cast<IfBlockStmt>(s.get())) {
            emitIfBlock(out, ib, currLineLabel, localCounter);
        } else if (auto ws2 = dyn_cast<WhileStmt>(s.get())) {
            emitWhile(out, ws2, currLineLabel, localCounter);
        } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Return -> " << ir; log(m.str()); }
        } else if (isa<EndStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body End -> " << ir; log(m.str()); }
        } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine); out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Goto -> " << ir; log(m.str()); }
        } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
            std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
            std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
            out << "  br label %" << entryLbl << "\n";
            emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
            out << contLbl << ":\n";
        } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
            ensureVarAllocated(out, ins->name);
            std::string fmt = nextTemp(); std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr @.fmt_in, i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Input -> " << ir1; log(m.str()); }
            std::string ir2 = "  call i32 (ptr, ...) @scanf(ptr "; ir2 += fmt; ir2 += ", ptr "; ir2 += varAllocaName_[ins->name]; ir2 += ")"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While body Input -> " << ir2; log(m.str()); }
        } else {
            throw CodeGenError("Unsupported statement in WHILE body");
        }
    }
    // Jump back to condition
    { std::string ir = "  br label %"; ir += condLbl; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " While -> " << ir; log(m.str()); } }
    // End label
    out << endLbl << ":\n";
}

} // namespace gwbasic

