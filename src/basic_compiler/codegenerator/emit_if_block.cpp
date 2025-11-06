// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include <sstream>
#include <format>

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
        out << br << Symbols::LF; log() << "line " << currentLine_ << " IfBlock -> " << br << Symbols::LF;
    } else {
        std::string br = "  br i1 "; br += cond; br += ", label %"; br += thenLbl; br += ", label %"; br += endLbl;
        out << br << Symbols::LF; log() << "line " << currentLine_ << " IfBlock -> " << br << Symbols::LF;
    }

    // THEN body
    out << thenLbl << ":" << Symbols::LF;
    bool thenTerminated = false;
    for (const auto& s : ib->thenBody) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
                std::string ir = "  "; ir += "store ptr "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
                out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Assign$ -> " << ir << Symbols::LF;
            } else {
                // Numeric assignment honors semantic numeric kind per variable
                storeNumberToVar(out, asg->name, val);
            }
        } else if (auto mid = dyn_cast<MidAssignStmt>(s.get())) {
            std::string dest;
            std::string storePtr;
            if (mid->index) {
                const int len = arraySizes_[mid->name];
                ensureStringArrayAllocated(out, mid->name, len);
                std::string base = arrayAllocaName_[mid->name];
                std::string idxReg = emitExpr(out, mid->index.get(), currLineLabel);
                std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; }
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << Symbols::LF; }
                storePtr = elem;
                dest = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", dest, elem); out << ir << Symbols::LF; }
            } else {
                ensureVarAllocated(out, mid->name);
                dest = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", dest, varAllocaName_[mid->name]); out << ir << Symbols::LF; }
                storePtr = varAllocaName_[mid->name];
            }
            std::string startD = emitExpr(out, mid->start.get(), currLineLabel);
            std::string startI = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", startI, startD); out << ir << Symbols::LF; }
            std::string off = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", off, startI); out << ir << Symbols::LF; }
            std::string dlen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", dlen, dest); out << ir << Symbols::LF; }
            std::string dsize = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", dsize, dlen); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, dsize); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strcpy(ptr {}, ptr {})", buf, dest); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store ptr {}, ptr {}", buf, storePtr); out << ir << Symbols::LF; }
            dest = buf;
            std::string src = emitExpr(out, mid->value.get(), currLineLabel);
            std::string slen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", slen, src); out << ir << Symbols::LF; }
            std::string n = slen;
            if (mid->len) {
                std::string lenD = emitExpr(out, mid->len.get(), currLineLabel);
                n = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n, lenD); out << ir << Symbols::LF; }
            }
            std::string negOff = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", negOff, off); out << ir << Symbols::LF; }
            std::string geLen = nextTemp(); { std::string ir = std::format("  {} = icmp sge i64 {}, {}", geLen, off, dlen); out << ir << Symbols::LF; }
            std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, negOff, geLen); out << ir << Symbols::LF; }
            std::string doLbl = currLineLabel + std::string("_mid_do_") + std::to_string(++localCounter);
            std::string endLbl2 = currLineLabel + std::string("_mid_end_") + std::to_string(localCounter);
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", bad, endLbl2, doLbl); out << ir << Symbols::LF; }
            out << doLbl << ":" << Symbols::LF;
            std::string avail = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", avail, dlen, off); out << ir << Symbols::LF; }
            std::string n_lt_av = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", n_lt_av, n, avail); out << ir << Symbols::LF; }
            std::string m1 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", m1, n_lt_av, n, avail); out << ir << Symbols::LF; }
            std::string m1_lt_s = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", m1_lt_s, m1, slen); out << ir << Symbols::LF; }
            std::string m2 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", m2, m1_lt_s, m1, slen); out << ir << Symbols::LF; }
            std::string dst = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", dst, dest, off); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", dst, src, m2); out << ir << Symbols::LF; }
            { std::string ir = std::format("  br label %{}", endLbl2); out << ir << Symbols::LF; }
            out << endLbl2 << ":" << Symbols::LF;
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
                    std::string fmt = nextTemp();
                    { std::string ir2 = "  "; ir2 += fmt; ir2 += " = getelementptr inbounds i8, ptr "; ir2 += (last ? "@.fmt_str" : "@.fmt_str_sp"); ir2 += ", i64 0"; out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Print -> " << ir2 << Symbols::LF; }
                    { std::string ir3 = "  call i32 (ptr, ...) @printf(ptr "; ir3 += fmt; ir3 += ", ptr "; ir3 += sptr; ir3 += ")"; out << ir3 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Print -> " << ir3 << Symbols::LF; }
                } else {
                    auto val = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp();
                    { std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += (last ? "@.fmt_num" : "@.fmt_num_sp"); ir1 += ", i64 0"; out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Print -> " << ir1 << Symbols::LF; }
                    { std::string ir2 = "  call i32 (ptr, ...) @printf(ptr "; ir2 += fmt; ir2 += ", double "; ir2 += val; ir2 += ")"; out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Print -> " << ir2 << Symbols::LF; }
                }
            }
        } else if (auto aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
            const int len = arraySizes_[aaset->name];
            std::string idxReg = emitExpr(out, aaset->index.get(), currLineLabel);
            std::string idxI64 = nextTemp(); { std::string ir = "  "; ir += idxI64; ir += " = fptosi double "; ir += idxReg; ir += " to i64"; out << ir << Symbols::LF; }
            if (isStringArrayNameCG(aaset->name)) {
                ensureStringArrayAllocated(out, aaset->name, len);
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << Symbols::LF; }
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                { std::string ir = std::format("  store ptr {}, ptr {}", val, elem); out << ir << Symbols::LF; }
            } else {
                ensureArrayAllocated(out, aaset->name, len);
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); { std::string ir = "  "; ir += elem; ir += " = getelementptr inbounds ["; ir += std::to_string(len); ir += " x double], ptr "; ir += base; ir += ", i64 0, i64 "; ir += idxI64; out << ir << Symbols::LF; }
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                { std::string ir = std::format("  store double {}, ptr {}", val, elem); out << ir << Symbols::LF; }
            }
        } else if (auto fs = dyn_cast<ForStmt>(s.get())) {
            emitFor(out, fs, currLineLabel, localCounter);
        } else if (auto ib2 = dyn_cast<IfBlockStmt>(s.get())) {
            emitIfBlock(out, ib2, currLineLabel, localCounter);
        } else if (auto rz = dyn_cast<RandomizeStmt>(s.get())) {
            if (rz->seed) {
                auto val = emitExpr(out, rz->seed.get(), currLineLabel);
                std::string si = nextTemp(); { std::string ir = "  "; ir += si; ir += " = fptosi double "; ir += val; ir += " to i64"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Randomize fptosi -> " << ir << Symbols::LF; }
                { std::string ir = "  call void @srand48(i64 "; ir += si; ir += ")"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Randomize srand48 -> " << ir << Symbols::LF; }
            } else {
                std::string t = nextTemp(); { std::string ir = "  "; ir += t; ir += " = call i64 @time(ptr null)"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Randomize time -> " << ir << Symbols::LF; }
                { std::string ir = "  call void @srand48(i64 "; ir += t; ir += ")"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Randomize srand48(time) -> " << ir << Symbols::LF; }
            }
            } else if (auto og = dyn_cast<OnGotoStmt>(s.get())) {
                std::string idx = emitExpr(out, og->index.get(), currLineLabel);
                std::string idxi32 = nextTemp(); { std::string ir = "  "; ir += idxi32; ir += " = fptosi double "; ir += idx; ir += " to i32"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then OnGoto fptosi -> " << ir << Symbols::LF; }
                std::string contLbl = currLineLabel + std::string("_on_cont_") + std::to_string(++localCounter);
                {
                    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < og->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << lineLabelName(og->targets[i]);
                    ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then OnGoto switch -> " << ir.str() << Symbols::LF;
                }
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ogs = dyn_cast<OnGosubStmt>(s.get())) {
                std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
                std::string idxi32 = nextTemp(); { std::string ir = "  "; ir += idxi32; ir += " = fptosi double "; ir += idx; ir += " to i32"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then OnGosub fptosi -> " << ir << Symbols::LF; }
                std::string contLbl = currLineLabel + std::string("_on_gs_cont_") + std::to_string(++localCounter);
                std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
                for (size_t i = 0; i < ogs->targets.size(); ++i) entryLbls.push_back(currLineLabel + std::string("_on_gs_entry_") + std::to_string(localCounter) + std::string("_") + std::to_string(i+1));
                {
                    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < ogs->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << entryLbls[i];
                    ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then OnGosub switch -> " << ir.str() << Symbols::LF;
                }
                for (size_t i = 0; i < ogs->targets.size(); ++i) emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
                out << contLbl << ":" << Symbols::LF;
            } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Return -> " << ir << Symbols::LF; thenTerminated = true; break;
            } else if (isa<EndStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then End -> " << ir << Symbols::LF; thenTerminated = true; break;
            } else if (isa<StopStmt>(s.get())) {
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_); out << ir << Symbols::LF; }
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Stop -> break+exit" << Symbols::LF; thenTerminated = true; break;
            } else if (isa<SystemStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then System -> " << ir << Symbols::LF; thenTerminated = true; break;
            } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine); out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Goto -> " << ir << Symbols::LF; thenTerminated = true; break;
        } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
            std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
            std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
            out << "  br label %" << entryLbl << Symbols::LF;
            emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
            out << contLbl << ":" << Symbols::LF;
        } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
            // Read into temp double then cast/store to variable's storage kind
            ensureVarAllocated(out, ins->name);
            std::string fmt = nextTemp();
            { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Input -> " << ir1 << Symbols::LF; }
            std::string tmp = nextTemp(); { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
            { std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock then Input -> " << ir2 << Symbols::LF; }
            std::string dv = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
            storeNumberToVar(out, ins->name, dv);
        } else {
            throw CodeGenError("Unsupported statement in IF body");
        }
    }
    // Branch to end when THEN body completes
    if (!thenTerminated) out << "  br label %" << endLbl << Symbols::LF;

    // ELSE body, if present
    if (!ib->elseBody.empty()) {
    out << elseLbl << ":" << Symbols::LF;
        bool elseTerminated = false;
        for (const auto& s : ib->elseBody) {
            if (auto asg = dyn_cast<AssignStmt>(s.get())) {
                std::string val = emitExpr(out, asg->value.get(), currLineLabel);
                if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
                    std::string ir = "  "; ir += "store ptr "; ir += val; ir += ", ptr "; ir += varAllocaName_[asg->name];
                    out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Assign$ -> " << ir << Symbols::LF;
                } else {
                    // Numeric assignment honors semantic numeric kind per variable
                    storeNumberToVar(out, asg->name, val);
                }
        } else if (auto mid = dyn_cast<MidAssignStmt>(s.get())) {
            std::string dest;
            std::string storePtr;
            if (mid->index) {
                const int len = arraySizes_[mid->name];
                ensureStringArrayAllocated(out, mid->name, len);
                std::string base = arrayAllocaName_[mid->name];
                std::string idxReg = emitExpr(out, mid->index.get(), currLineLabel);
                std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; }
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << Symbols::LF; }
                storePtr = elem;
                dest = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", dest, elem); out << ir << Symbols::LF; }
            } else {
                ensureVarAllocated(out, mid->name);
                dest = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", dest, varAllocaName_[mid->name]); out << ir << Symbols::LF; }
                storePtr = varAllocaName_[mid->name];
            }
                std::string startD = emitExpr(out, mid->start.get(), currLineLabel);
                std::string startI = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", startI, startD); out << ir << Symbols::LF; }
                std::string off = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, 1", off, startI); out << ir << Symbols::LF; }
            std::string dlen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", dlen, dest); out << ir << Symbols::LF; }
            std::string dsize = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", dsize, dlen); out << ir << Symbols::LF; }
            std::string buf = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", buf, dsize); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strcpy(ptr {}, ptr {})", buf, dest); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store ptr {}, ptr {}", buf, storePtr); out << ir << Symbols::LF; }
            dest = buf;
                std::string src = emitExpr(out, mid->value.get(), currLineLabel);
                std::string slen = nextTemp(); { std::string ir = std::format("  {} = call i64 @strlen(ptr {})", slen, src); out << ir << Symbols::LF; }
                std::string n = slen;
                if (mid->len) {
                    std::string lenD = emitExpr(out, mid->len.get(), currLineLabel);
                    n = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", n, lenD); out << ir << Symbols::LF; }
                }
                std::string negOff = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, 0", negOff, off); out << ir << Symbols::LF; }
                std::string geLen = nextTemp(); { std::string ir = std::format("  {} = icmp sge i64 {}, {}", geLen, off, dlen); out << ir << Symbols::LF; }
                std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, negOff, geLen); out << ir << Symbols::LF; }
                std::string doLbl = currLineLabel + std::string("_mid_do_") + std::to_string(++localCounter);
                std::string endLbl2 = currLineLabel + std::string("_mid_end_") + std::to_string(localCounter);
                { std::string ir = std::format("  br i1 {}, label %{}, label %{}", bad, endLbl2, doLbl); out << ir << Symbols::LF; }
                out << doLbl << ":" << Symbols::LF;
                std::string avail = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", avail, dlen, off); out << ir << Symbols::LF; }
                std::string n_lt_av = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", n_lt_av, n, avail); out << ir << Symbols::LF; }
                std::string m1 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", m1, n_lt_av, n, avail); out << ir << Symbols::LF; }
                std::string m1_lt_s = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", m1_lt_s, m1, slen); out << ir << Symbols::LF; }
                std::string m2 = nextTemp(); { std::string ir = std::format("  {} = select i1 {}, i64 {}, i64 {}", m2, m1_lt_s, m1, slen); out << ir << Symbols::LF; }
                std::string dst = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", dst, dest, off); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", dst, src, m2); out << ir << Symbols::LF; }
                { std::string ir = std::format("  br label %{}", endLbl2); out << ir << Symbols::LF; }
                out << endLbl2 << ":" << Symbols::LF;
            } else if (auto pr = dyn_cast<PrintStmt>(s.get())) {
                std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
                for (size_t pi = 0; pi < items.size(); ++pi) {
                    const bool last = (pi + 1 == items.size());
                    const Expr* v = items[pi];
                    if (isa<StringExpr>(v)) {
                        int id = strLiteralId_[dyn_cast<StringExpr>(v)->value];
                        std::string sptr = nextTemp(); { std::string ir1 = "  "; ir1 += sptr; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += globalStringName(id); ir1 += ", i64 0"; out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Print -> " << ir1 << Symbols::LF; }
                        std::string fmt = nextTemp(); { std::string ir2 = "  "; ir2 += fmt; ir2 += " = getelementptr inbounds i8, ptr "; ir2 += (last ? "@.fmt_str" : "@.fmt_str_sp"); ir2 += ", i64 0"; out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Print -> " << ir2 << Symbols::LF; }
                        { std::string ir3 = "  call i32 (ptr, ...) @printf(ptr "; ir3 += fmt; ir3 += ", ptr "; ir3 += sptr; ir3 += ")"; out << ir3 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Print -> " << ir3 << Symbols::LF; }
                    } else {
                        auto val = emitExpr(out, v, currLineLabel);
                        std::string fmt = nextTemp(); { std::string ir1 = "  "; ir1 += fmt; ir1 += " = getelementptr inbounds i8, ptr "; ir1 += (last ? "@.fmt_num" : "@.fmt_num_sp"); ir1 += ", i64 0"; out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Print -> " << ir1 << Symbols::LF; }
                        { std::string ir2 = "  call i32 (ptr, ...) @printf(ptr "; ir2 += fmt; ir2 += ", double "; ir2 += val; ir2 += ")"; out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Print -> " << ir2 << Symbols::LF; }
                    }
                }
            } else if (auto fs = dyn_cast<ForStmt>(s.get())) {
                emitFor(out, fs, currLineLabel, localCounter);
            } else if (auto ib2 = dyn_cast<IfBlockStmt>(s.get())) {
                emitIfBlock(out, ib2, currLineLabel, localCounter);
            } else if (auto rz = dyn_cast<RandomizeStmt>(s.get())) {
                if (rz->seed) {
                    auto val = emitExpr(out, rz->seed.get(), currLineLabel);
                    std::string si = nextTemp(); { std::string ir = "  "; ir += si; ir += " = fptosi double "; ir += val; ir += " to i64"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Randomize fptosi -> " << ir << Symbols::LF; }
                    { std::string ir = "  call void @srand48(i64 "; ir += si; ir += ")"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Randomize srand48 -> " << ir << Symbols::LF; }
                } else {
                    std::string t = nextTemp(); { std::string ir = "  "; ir += t; ir += " = call i64 @time(ptr null)"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Randomize time -> " << ir << Symbols::LF; }
                    { std::string ir = "  call void @srand48(i64 "; ir += t; ir += ")"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Randomize srand48(time) -> " << ir << Symbols::LF; }
                }
            } else if (auto og = dyn_cast<OnGotoStmt>(s.get())) {
                std::string idx = emitExpr(out, og->index.get(), currLineLabel);
                std::string idxi32 = nextTemp(); { std::string ir = "  "; ir += idxi32; ir += " = fptosi double "; ir += idx; ir += " to i32"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else OnGoto fptosi -> " << ir << Symbols::LF; }
                std::string contLbl = currLineLabel + std::string("_on_cont_") + std::to_string(++localCounter);
                {
                    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < og->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << lineLabelName(og->targets[i]);
                    ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else OnGoto switch -> " << ir.str() << Symbols::LF;
                }
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ogs = dyn_cast<OnGosubStmt>(s.get())) {
                std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
                std::string idxi32 = nextTemp(); { std::string ir = "  "; ir += idxi32; ir += " = fptosi double "; ir += idx; ir += " to i32"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else OnGosub fptosi -> " << ir << Symbols::LF; }
                std::string contLbl = currLineLabel + std::string("_on_gs_cont_") + std::to_string(++localCounter);
                std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
                for (size_t i = 0; i < ogs->targets.size(); ++i) entryLbls.push_back(currLineLabel + std::string("_on_gs_entry_") + std::to_string(localCounter) + std::string("_") + std::to_string(i+1));
                {
                    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
                    for (size_t i = 0; i < ogs->targets.size(); ++i) ir << " i32 " << (i+1) << ", label %" << entryLbls[i];
                    ir << " ]"; out << ir.str() << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else OnGosub switch -> " << ir.str() << Symbols::LF;
                }
                for (size_t i = 0; i < ogs->targets.size(); ++i) emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
                out << contLbl << ":" << Symbols::LF;
            } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Return -> " << ir << Symbols::LF; elseTerminated = true; break;
            } else if (isa<EndStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else End -> " << ir << Symbols::LF; elseTerminated = true; break;
            } else if (isa<StopStmt>(s.get())) {
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_); out << ir << Symbols::LF; }
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Stop -> break+exit" << Symbols::LF; elseTerminated = true; break;
            } else if (isa<SystemStmt>(s.get())) {
            std::string ir = "  br label %exit"; out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else System -> " << ir << Symbols::LF; elseTerminated = true; break;
            } else if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine); out << ir << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Goto -> " << ir << Symbols::LF; elseTerminated = true; break;
            } else if (auto gs = dyn_cast<GosubStmt>(s.get())) {
                std::string contLbl = currLineLabel; contLbl += "_gosub_cont"; contLbl += std::to_string(++localCounter);
                std::string entryLbl = currLineLabel; entryLbl += "_gosub_entry"; entryLbl += std::to_string(localCounter);
                out << "  br label %" << entryLbl << Symbols::LF;
                emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
                out << contLbl << ":" << Symbols::LF;
            } else if (auto ins = dyn_cast<InputStmt>(s.get())) {
                // Read into temp double then cast/store to variable's storage kind
                ensureVarAllocated(out, ins->name);
                std::string fmt = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Input -> " << ir1 << Symbols::LF; }
                std::string tmp = nextTemp(); { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
                { std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp); out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " IfBlock else Input -> " << ir2 << Symbols::LF; }
                std::string dv = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
                storeNumberToVar(out, ins->name, dv);
            } else {
                throw CodeGenError("Unsupported statement in IF body");
            }
        }
        if (!elseTerminated) out << "  br label %" << endLbl << Symbols::LF;
    }

    out << endLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
