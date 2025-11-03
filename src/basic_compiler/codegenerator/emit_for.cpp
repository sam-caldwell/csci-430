// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include <format>
#include <sstream>

namespace gwbasic {

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
void CodeGenerator::emitFor(std::ostringstream& out, const ForStmt* fs, const std::string& currLineLabel, int& localCounter) {

    std::string loopId = std::to_string(++localCounter);
    std::string condLbl = currLineLabel; condLbl += "_for_cond"; condLbl += loopId;
    std::string bodyLbl = currLineLabel; bodyLbl += "_for_body"; bodyLbl += loopId;
    std::string incLbl  = currLineLabel; incLbl  += "_for_inc";  incLbl  += loopId;
    std::string endLbl  = currLineLabel; endLbl  += "_for_end";  endLbl  += loopId;

    ensureVarAllocated(out, fs->var);
    {
        std::string startReg = emitExpr(out, fs->start.get(), currLineLabel);
        std::string ir1 = std::format("  store double {}, ptr {}", startReg, varAllocaName_[fs->var]);
        out << ir1 << STR_LF; log() << "line " << currentLine_ << " ForStmt init -> " << ir1 << CH_LF;
        std::string ir2 = std::format("  br label %{}", condLbl);
        out << ir2 << STR_LF; log() << "line " << currentLine_ << " ForStmt -> " << ir2 << CH_LF;
    }

    out << condLbl << ":" << STR_LF;
    std::string curVal = nextTemp();
    {
        std::string ir = std::format("  {} = load double, ptr {}", curVal, varAllocaName_[fs->var]);
        out << ir << STR_LF;
        {
            log() << "line " << currentLine_ << " ForStmt cond load -> " << ir << CH_LF;
        }
    }
    {
        // Evaluate end and step for condition decision
        std::string endReg = emitExpr(out, fs->end.get(), currLineLabel);
        std::string stepReg = fs->step ? emitExpr(out, fs->step.get(), currLineLabel) : std::string("1.0");
        std::string isNeg = nextTemp();
        { std::string ir = std::format("  {} = fcmp olt double {}, 0.0", isNeg, stepReg); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt step<0 -> " << ir << CH_LF; }
        std::string condLe = nextTemp();
        { std::string ir = std::format("  {} = fcmp ole double {}, {}", condLe, curVal, endReg); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt cond <= -> " << ir << CH_LF; }
        std::string condGe = nextTemp();
        { std::string ir = std::format("  {} = fcmp oge double {}, {}", condGe, curVal, endReg); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt cond >= -> " << ir << CH_LF; }
        std::string cond = nextTemp();
        { std::string ir = std::format("  {} = select i1 {}, i1 {}, i1 {}", cond, isNeg, condGe, condLe); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt select cond -> " << ir << CH_LF; }
        std::string br = std::format("  br i1 {}, label %{}, label %{}", cond, bodyLbl, endLbl);
        out << br << STR_LF; log() << "line " << currentLine_ << " ForStmt branch -> " << br << CH_LF;
    }

    out << bodyLbl << ":" << STR_LF;
    for (const auto& s : fs->body) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            std::string ir;
            if (!asg->name.empty() && asg->name.back() == CH_DOLLARSIGN) { ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]); }
            else { ir = std::format("  store double {}, ptr {}", val, varAllocaName_[asg->name]); }
            out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt body Assign -> " << ir << CH_LF;
        } else if (auto pr = dyn_cast<PrintStmt>(s.get())) {
            std::vector<const Expr*> items;
            if (pr->value) items.push_back(pr->value.get());
            for (const auto& v : pr->more) items.push_back(v.get());
            for (size_t pi = 0; pi < items.size(); ++pi) {
                const bool last = (pi + 1 == items.size());
                const Expr* v = items[pi];
                auto isStr = [&](const Expr* e, const auto& self) -> bool {
                    if (isa<StringExpr>(e)) return true;
                    if (auto vv = dyn_cast<VarExpr>(e)) return !vv->name.empty() && vv->name.back() == CH_DOLLARSIGN;
                    if (auto bb = dyn_cast<BinaryExpr>(e)) return (bb->op == BinaryOp::Add) && (self(bb->lhs.get(), self) || self(bb->rhs.get(), self));
                    return false;
                };
                if (isStr(v, isStr)) {
                    auto sptr = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp();
                    { std::string ir2 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")); out << ir2 << STR_LF; log() << "line " << currentLine_ << " ForStmt body Print -> " << ir2 << CH_LF; }
                    { std::string ir3 = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, sptr); out << ir3 << STR_LF; log() << "line " << currentLine_ << " ForStmt body Print -> " << ir3 << CH_LF; }
                } else {
                    auto val = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp();
                    { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_num" : "@.fmt_num_sp")); out << ir1 << STR_LF; log() << "line " << currentLine_ << " ForStmt body Print -> " << ir1 << CH_LF; }
                    { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmt, val); out << ir2 << STR_LF; log() << "line " << currentLine_ << " ForStmt body Print -> " << ir2 << CH_LF; }
                }
            }
        } else if (auto aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
            const int len = arraySizes_[aaset->name]; ensureArrayAllocated(out, aaset->name, len);
            std::string base = arrayAllocaName_[aaset->name];
            std::string idxReg = emitExpr(out, aaset->index.get(), currLineLabel);
            std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt body idx -> " << ir << CH_LF; }
            std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x double], ptr {}, i64 0, i64 {}", elem, len, base, idxI64); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt body gep -> " << ir << CH_LF; }
            std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
            { std::string ir = std::format("  store double {}, ptr {}", val, elem); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt body store -> " << ir << CH_LF; }
        } else {
            throw CodeGenError("Unsupported statement in FOR body");
        }
    }
    out << "  br label %" << incLbl << STR_LF;

    out << incLbl << ":" << STR_LF;
    std::string stepReg = fs->step ? emitExpr(out, fs->step.get(), currLineLabel) : std::string("1.0");
    std::string vcur = nextTemp();
    { std::string ir = std::format("  {} = load double, ptr {}", vcur, varAllocaName_[fs->var]); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt inc load -> " << ir << CH_LF; }
    std::string vnext = nextTemp();
    { std::string ir = std::format("  {} = fadd double {}, {}", vnext, vcur, stepReg); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt inc add -> " << ir << CH_LF; }
    { std::string ir = std::format("  store double {}, ptr {}", vnext, varAllocaName_[fs->var]); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt inc store -> " << ir << CH_LF; }
    { std::string ir = std::format("  br label %{}", condLbl); out << ir << STR_LF; log() << "line " << currentLine_ << " ForStmt -> " << ir << CH_LF; }

    out << endLbl << ":" << STR_LF;
}

} // namespace gwbasic
