// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include <sstream>
#include <format>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitWhile
 * Inputs:
 *  - out: IR output stream to append to
 *  - ws: Parsed WhileStmt node
 *  - currLineLabel: Base label for naming emitted blocks
 *  - localCounter: Per-line counter to uniquify labels
 * Outputs:
 *  - void (appends IR for WHILE loop structure)
 * Theory of operation:
 *  - Emits a loop with condition, body, and back-edge, merging at an end
 *    label. The condition must be a comparison expression.
 */
void CodeGenerator::emitWhile(std::ostringstream& out, const WhileStmt* ws, const std::string& currLineLabel, int& localCounter) {
    std::string id = std::to_string(++localCounter);
    std::string condLbl = currLineLabel; condLbl += "_while_cond"; condLbl += id;
    std::string bodyLbl = currLineLabel; bodyLbl += "_while_body"; bodyLbl += id;
    std::string endLbl  = currLineLabel; endLbl  += "_while_end";  endLbl  += id;

    // Jump to condition
    { std::string ir = std::format("  br label %{}", condLbl); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While -> " << ir << Symbols::LF; }

    // Condition
    out << condLbl << ":" << Symbols::LF;
    auto be = dyn_cast<const BinaryExpr>(ws->cond.get());
    if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt && be->op != BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) {
        throw CodeGenError("WHILE condition must be a comparison");
    }
    std::string cond = emitComparison(out, be);
    { std::string ir = std::format("  br i1 {}, label %{}, label %{}", cond, bodyLbl, endLbl); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While branch -> " << ir << Symbols::LF; }

    // Body
    out << bodyLbl << ":" << Symbols::LF;
    for (const auto& s : ws->body) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            std::string ir;
            if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) { ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]); }
            else { ir = std::format("  store double {}, ptr {}", val, varAllocaName_[asg->name]); }
            out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body Assign -> " << ir << Symbols::LF;
        } else if (auto pr = dyn_cast<PrintStmt>(s.get())) {
            std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
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
                    auto sptr = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp(); { std::string ir2 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Print -> " << ir2 << Symbols::LF; }
                    { std::string ir3 = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, sptr); out << ir3 << Symbols::LF; log() << "line " << currentLine_ << " While body Print -> " << ir3 << Symbols::LF; }
                } else {
                    auto val = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_num" : "@.fmt_num_sp")); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " While body Print -> " << ir1 << Symbols::LF; }
                    { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmt, val); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Print -> " << ir2 << Symbols::LF; }
                }
            }
        } else if (auto aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
            const int len = arraySizes_[aaset->name]; ensureArrayAllocated(out, aaset->name, len);
            std::string base = arrayAllocaName_[aaset->name];
            std::string idxReg = emitExpr(out, aaset->index.get(), currLineLabel);
            std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; }
            std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << Symbols::LF; }
            std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
            { std::string ir = std::format("  store double {}, ptr {}", val, elem); out << ir << Symbols::LF; }
        } else if (auto fs = dyn_cast<ForStmt>(s.get())) {
            emitFor(out, fs, currLineLabel, localCounter);
        } else if (auto ib = dyn_cast<IfBlockStmt>(s.get())) {
            emitIfBlock(out, ib, currLineLabel, localCounter);
        } else if (auto ws2 = dyn_cast<WhileStmt>(s.get())) {
            emitWhile(out, ws2, currLineLabel, localCounter);
        } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body Return -> " << ir << Symbols::LF;
        } else if (isa<EndStmt>(s.get())) {
            std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body End -> " << ir << Symbols::LF;
        } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = std::format("  br label %{}", lineLabelName(gt->targetLine)); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While body Goto -> " << ir << Symbols::LF;
        } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
            std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
            std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
            out << std::format("  br label %{}", entryLbl) << Symbols::LF;
            emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
            out << contLbl << ":" << Symbols::LF;
        } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
            ensureVarAllocated(out, ins->name);
            std::string fmt = nextTemp(); std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " While body Input -> " << ir1 << Symbols::LF;
            std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, varAllocaName_[ins->name]); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " While body Input -> " << ir2 << Symbols::LF;
        } else {
            throw CodeGenError("Unsupported statement in WHILE body");
        }
    }
    // Jump back to condition
    { std::string ir = std::format("  br label %{}", condLbl); out << ir << Symbols::LF; log() << "line " << currentLine_ << " While -> " << ir << Symbols::LF; }
    // End label
    out << endLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
