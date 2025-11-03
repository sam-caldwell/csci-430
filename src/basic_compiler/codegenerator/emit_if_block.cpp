// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitIfBlock
 * Inputs:
 *  - out: IR output stream to append to
 *  - ib: Parsed IfBlockStmt node (with optional ELSE body)
 *  - currLineLabel: Base label for naming emitted blocks
 *  - localCounter: Per-line counter to uniquify labels
 * Outputs:
 *  - void (appends IR for IF/THEN[/ELSE] block)
 * Theory of operation:
 *  - Lowers the condition as a comparison, emits branches to THEN/ELSE,
 *    lowers contained statements, and merges control at an end label.
 */
void CodeGenerator::emitIfBlock(std::ostringstream& out, const IfBlockStmt* ib, const std::string& currLineLabel, int& localCounter) {
    std::string ifId = std::to_string(++localCounter);
    std::string thenLbl = currLineLabel; thenLbl += "_if_then"; thenLbl += ifId;
    std::string elseLbl = currLineLabel; elseLbl += "_if_else"; elseLbl += ifId;
    std::string endLbl  = currLineLabel; endLbl  += "_if_end";  endLbl  += ifId;

    // Emit condition
    auto be = dyn_cast<const BinaryExpr>(ib->cond.get());
    if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt && be->op != BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) {
        throw CodeGenError("IF condition must be a comparison");
    }
    std::string cond = emitComparison(out, be);
    if (!ib->elseBody.empty()) {
        std::string br = "  br i1 "; br += cond; br += ", label %"; br += thenLbl; br += ", label %"; br += elseLbl;
        out << br << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock -> " << br; log(m.str()); }
    } else {
        std::string br = "  br i1 "; br += cond; br += ", label %"; br += thenLbl; br += ", label %"; br += endLbl;
        out << br << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock -> " << br; log(m.str()); }
    }

    // THEN body
    out << thenLbl << ":\n";
    for (const auto& s : ib->thenBody) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            std::string ir = "  store double "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
            out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Assign -> " << ir; log(m.str()); }
        } else if (auto pr = dyn_cast<PrintStmt>(s.get())) {
            std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
            for (size_t pi = 0; pi < items.size(); ++pi) {
                const bool last = (pi + 1 == items.size());
                const Expr* v = items[pi];
                if (isa<StringExpr>(v)) {
                    int id = strLiteralId_[dyn_cast<StringExpr>(v)->value];
                    std::string sptr = nextTemp();
                    { std::string ir1 = "  "; ir1 += sptr; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += globalStringName(id); ir1 += ", i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Print -> " << ir1; log(m.str()); } }
                    std::string fmt = nextTemp();
                    { std::string ir2 = "  "; ir2 += fmt; ir2 += " = getelementptr inbounds i8, ptr "; ir2 += (last ? "@.fmt_str" : "@.fmt_str_sp"); ir2 += ", i64 0"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Print -> " << ir2; log(m.str()); } }
                    { std::string ir3 = "  call i32 (ptr, ...) @printf(ptr "; ir3 += fmt; ir3 += ", ptr "; ir3 += sptr; ir3 += ")"; out << ir3 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Print -> " << ir3; log(m.str()); } }
                } else {
                    auto val = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp();
                    { std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += (last ? "@.fmt_num" : "@.fmt_num_sp"); ir1 += ", i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Print -> " << ir1; log(m.str()); } }
                    { std::string ir2 = "  call i32 (ptr, ...) @printf(ptr "; ir2 += fmt; ir2 += ", double "; ir2 += val; ir2 += ")"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Print -> " << ir2; log(m.str()); } }
                }
            }
        } else if (auto fs = dyn_cast<ForStmt>(s.get())) {
            emitFor(out, fs, currLineLabel, localCounter);
        } else if (auto ib2 = dyn_cast<IfBlockStmt>(s.get())) {
            emitIfBlock(out, ib2, currLineLabel, localCounter);
        } else if (auto rz = dyn_cast<RandomizeStmt>(s.get())) {
            if (rz->seed) {
                auto val = emitExpr(out, rz->seed.get(), currLineLabel);
                std::string si = nextTemp(); { std::string ir = "  "; ir += si; ir += " = fptosi double "; ir += val; ir += " to i64"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Randomize fptosi -> " << ir; log(m.str()); } }
                { std::string ir = "  call void @srand48(i64 "; ir += si; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Randomize srand48 -> " << ir; log(m.str()); } }
            } else {
                std::string t = nextTemp(); { std::string ir = "  "; ir += t; ir += " = call i64 @time(ptr null)"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Randomize time -> " << ir; log(m.str()); } }
                { std::string ir = "  call void @srand48(i64 "; ir += t; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Randomize srand48(time) -> " << ir; log(m.str()); } }
            }
        } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Return -> " << ir; log(m.str()); }
        } else if (isa<EndStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then End -> " << ir; log(m.str()); }
        } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine); out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Goto -> " << ir; log(m.str()); }
        } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
            std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
            std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
            out << "  br label %" << entryLbl << "\n";
            emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
            out << contLbl << ":\n";
        } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
            ensureVarAllocated(out, ins->name);
            std::string fmt = nextTemp();
            std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr @.fmt_in, i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Input -> " << ir1; log(m.str()); }
            std::string ir2 = "  call i32 (ptr, ...) @scanf(ptr "; ir2 += fmt; ir2 += ", ptr "; ir2 += varAllocaName_[ins->name]; ir2 += ")"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock then Input -> " << ir2; log(m.str()); }
        } else {
            throw CodeGenError("Unsupported statement in IF body");
        }
    }
    // Branch to end when THEN body completes
    out << "  br label %" << endLbl << "\n";

    // ELSE body, if present
    if (!ib->elseBody.empty()) {
        out << elseLbl << ":\n";
        for (const auto& s : ib->elseBody) {
            if (auto asg = dyn_cast<AssignStmt>(s.get())) {
                std::string val = emitExpr(out, asg->value.get(), currLineLabel);
                std::string ir = "  store double "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
                out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Assign -> " << ir; log(m.str()); }
            } else if (auto pr = dyn_cast<PrintStmt>(s.get())) {
                std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
                for (size_t pi = 0; pi < items.size(); ++pi) {
                    const bool last = (pi + 1 == items.size());
                    const Expr* v = items[pi];
                    if (isa<StringExpr>(v)) {
                        int id = strLiteralId_[dyn_cast<StringExpr>(v)->value];
                        std::string sptr = nextTemp(); { std::string ir1 = "  "; ir1 += sptr; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += globalStringName(id); ir1 += ", i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Print -> " << ir1; log(m.str()); } }
                        std::string fmt = nextTemp(); { std::string ir2 = "  "; ir2 += fmt; ir2 += " = getelementptr inbounds i8, ptr "; ir2 += (last ? "@.fmt_str" : "@.fmt_str_sp"); ir2 += ", i64 0"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Print -> " << ir2; log(m.str()); } }
                        { std::string ir3 = "  call i32 (ptr, ...) @printf(ptr "; ir3 += fmt; ir3 += ", ptr "; ir3 += sptr; ir3 += ")"; out << ir3 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Print -> " << ir3; log(m.str()); } }
                    } else {
                        auto val = emitExpr(out, v, currLineLabel);
                        std::string fmt = nextTemp(); { std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += (last ? "@.fmt_num" : "@.fmt_num_sp"); ir1 += ", i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Print -> " << ir1; log(m.str()); } }
                        { std::string ir2 = "  call i32 (ptr, ...) @printf(ptr "; ir2 += fmt; ir2 += ", double "; ir2 += val; ir2 += ")"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Print -> " << ir2; log(m.str()); } }
                    }
                }
            } else if (auto fs = dyn_cast<ForStmt>(s.get())) {
                emitFor(out, fs, currLineLabel, localCounter);
            } else if (auto ib2 = dyn_cast<IfBlockStmt>(s.get())) {
                emitIfBlock(out, ib2, currLineLabel, localCounter);
            } else if (auto rz = dyn_cast<RandomizeStmt>(s.get())) {
                if (rz->seed) {
                    auto val = emitExpr(out, rz->seed.get(), currLineLabel);
                    std::string si = nextTemp(); { std::string ir = "  "; ir += si; ir += " = fptosi double "; ir += val; ir += " to i64"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Randomize fptosi -> " << ir; log(m.str()); } }
                    { std::string ir = "  call void @srand48(i64 "; ir += si; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Randomize srand48 -> " << ir; log(m.str()); } }
                } else {
                    std::string t = nextTemp(); { std::string ir = "  "; ir += t; ir += " = call i64 @time(ptr null)"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Randomize time -> " << ir; log(m.str()); } }
                    { std::string ir = "  call void @srand48(i64 "; ir += t; ir += ")"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Randomize srand48(time) -> " << ir; log(m.str()); } }
                }
            } else if (isa<ReturnStmt>(s.get())) {
                std::string ir = "  br label %exit"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Return -> " << ir; log(m.str()); }
            } else if (isa<EndStmt>(s.get())) {
                std::string ir = "  br label %exit"; out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else End -> " << ir; log(m.str()); }
            } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
                std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine); out << ir << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Goto -> " << ir; log(m.str()); }
            } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
                std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
                std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
                out << "  br label %" << entryLbl << "\n";
                emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
                out << contLbl << ":\n";
            } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
                ensureVarAllocated(out, ins->name);
                std::string fmt = nextTemp(); std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr @.fmt_in, i64 0"; out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Input -> " << ir1; log(m.str()); }
                std::string ir2 = "  call i32 (ptr, ...) @scanf(ptr "; ir2 += fmt; ir2 += ", ptr "; ir2 += varAllocaName_[ins->name]; ir2 += ")"; out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " IfBlock else Input -> " << ir2; log(m.str()); }
            } else {
                throw CodeGenError("Unsupported statement in IF body");
            }
        }
        out << "  br label %" << endLbl << "\n";
    }

    out << endLbl << ":\n";
}

} // namespace gwbasic
