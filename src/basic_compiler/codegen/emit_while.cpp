// (c) 2025 Sam Caldwell. All Rights Reserved.
// NOLINTBEGIN(llvm-include-order,misc-include-cleaner)
#include "basic_compiler/codegen/CodeGenerator.h"

#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/ColorStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/codegen/CodeGenError.h"

#include <cmath>
#include <algorithm>
#include <format>
#include <memory>
#include <sstream>
#include <string>
#include <cstddef>
#include <vector>
// NOLINTEND(llvm-include-order,misc-include-cleaner)

namespace gwbasic {

/*
 * Function: emitWhile
 * Summary: Emit IR for a WHILE loop statement.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - while_stmt: Parsed WhileStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void (appends IR for WHILE loop structure)
 * Theory of operation:
 *  - Emit a loop with condition, body, and back-edge, merging at an end
 *    label. The condition may be comparison or general expression.
 */
// NOLINTBEGIN(readability-function-cognitive-complexity,readability-function-size,readability-identifier-length,readability-avoid-nested-conditional-operator,readability-braces-around-statements,misc-const-correctness)
void CodeGenerator::emitWhile(std::ostringstream& out, const WhileStmt* while_stmt, const std::string& currLineLabel, int& localCounter) {
    std::string id = std::to_string(++localCounter);
    std::string condLbl = currLineLabel; condLbl += "_while_cond"; condLbl += id;
    std::string bodyLbl = currLineLabel; bodyLbl += "_while_body"; bodyLbl += id;
    std::string endLbl  = currLineLabel; endLbl  += "_while_end";  endLbl  += id;

    // Jump to condition
    out << std::format("  br label %{}", condLbl) << Symbols::LF;
    

    // Condition
    out << condLbl << ":" << Symbols::LF;
    std::string cond;
    if (const auto* be = dyn_cast<const BinaryExpr>(while_stmt->cond.get());
        be != nullptr && (be->op == BinaryOp::Eq || be->op == BinaryOp::Ne || be->op == BinaryOp::Lt || be->op == BinaryOp::Le || be->op == BinaryOp::Gt || be->op == BinaryOp::Ge)) {
        cond = emitComparison(out, be);
    } else {
        std::string val = emitExpr(out, while_stmt->cond.get(), "");
        cond = nextTemp();
        out << std::format("  {} = fcmp one double {}, 0.0", cond, val) << Symbols::LF;
    }
    out << std::format("  br i1 {}, label %{}, label %{}", cond, bodyLbl, endLbl) << Symbols::LF;
    

    // Body
    out << bodyLbl << ":" << Symbols::LF;
    for (const auto& s : while_stmt->body) {
        if (const auto* asg = dyn_cast<AssignStmt>(s.get())) {
            std::string val = emitExpr(out, asg->value.get(), currLineLabel);
            if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
                std::string ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
                out << ir << Symbols::LF;
            } else {
                storeNumberToVar(out, asg->name, val);
            }
        } else if (const auto* mid = dyn_cast<MidAssignStmt>(s.get())) {
            std::string dest;
            std::string storePtr;
            if (!mid->indices.empty()) {
                const auto &dims = arrayDims_[mid->name];
                long long total = 1; for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); ext = std::max<long long>(ext, 0); total *= ext; }
                ensureStringArrayAllocated(out, mid->name, static_cast<int>(total));
                std::string base = arrayAllocaName_[mid->name];
                std::vector<std::string> idxI64s; idxI64s.reserve(mid->indices.size());
                std::vector<std::string> bads; bads.reserve(mid->indices.size());
                for (size_t di = 0; di < mid->indices.size(); ++di) {
                    std::string idxReg = emitExpr(out, mid->indices[di].get(), currLineLabel);
                    std::string idxI64 = nextTemp(); out << std::format("  {} = fptosi double {} to i64", idxI64, idxReg) << Symbols::LF;
                    idxI64s.push_back(idxI64);
                    std::string ltBase = nextTemp(); out << std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI64, optionBase_) << Symbols::LF;
                    std::string gtUb = nextTemp(); out << std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI64, dims[di]) << Symbols::LF;
                    std::string bad = nextTemp(); out << std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb) << Symbols::LF;
                    bads.push_back(bad);
                }
                std::string anyBad = bads[0];
                for (size_t i = 1; i < bads.size(); ++i) { std::string nb = nextTemp(); out << std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]) << Symbols::LF; anyBad = nb; }
                std::string doLbl = std::format("{}_while_mid_ok_{}", currLineLabel, ++localCounter);
                std::string errLbl = std::format("{}_while_mid_err_{}", currLineLabel, localCounter);
                out << std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl) << Symbols::LF;
                out << errLbl << ":" << Symbols::LF;
                out << std::format("  store i32 9, ptr @gwb_err_code") << Symbols::LF;
                out << std::format("  store i32 {}, ptr @gwb_err_line", currentLine_) << Symbols::LF;
                out << std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_) << Symbols::LF;
                out << std::format("  store i32 0, ptr @gwb_resume_stmt") << Symbols::LF;
                out << std::format("  store i1 true, ptr @gwb_in_handler") << Symbols::LF;
                // Mirror into ERR/ERL variables for runtime bounds errors
                ensureVarAllocated(out, "ERR");
                ensureVarAllocated(out, "ERL");
                { std::string derr = nextTemp(); out << std::format("  {} = sitofp i32 9 to double", derr) << Symbols::LF; storeNumberToVar(out, "ERR", derr); }
                { std::string dln = nextTemp(); out << std::format("  {} = sitofp i32 {} to double", dln, currentLine_) << Symbols::LF; storeNumberToVar(out, "ERL", dln); }
                {
                    std::string trap = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF;
                    out << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
                    for (int lnum : lineNumbers_) { out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF; }
                    out << "  ]" << Symbols::LF;
                }
                out << doLbl << ":" << Symbols::LF;
                std::vector<long long> extents; extents.reserve(dims.size());
                for (size_t di = 0; di < dims.size(); ++di) { long long e = static_cast<long long>(dims[di]) - optionBase_ + 1; e = std::max<long long>(e, 0); extents.push_back(e); }
                std::vector<long long> strides(dims.size(), 1);
                for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) { strides[di] = strides[di + 1] * extents[di + 1]; }
                std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
                for (const auto& ii : idxI64s) { std::string a = nextTemp(); { std::string ir = std::format("  {} = sub i64 {}, {}", a, ii, optionBase_); out << ir << Symbols::LF; } adjs.push_back(a); }
                std::string lin = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]); out << ir << Symbols::LF; }
                for (size_t di = 1; di < adjs.size(); ++di) { std::string t = nextTemp(); { std::string ir = std::format("  {} = mul i64 {}, {}", t, adjs[di], strides[di]); out << ir << Symbols::LF; } std::string s2 = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, {}", s2, lin, t); out << ir << Symbols::LF; } lin = s2; }
                std::string elem = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin); out << ir << Symbols::LF; }
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
            std::string doLbl = std::format("{}_mid_do_{}", currLineLabel, ++localCounter);
            std::string endLbl2 = std::format("{}_mid_end_{}", currLineLabel, localCounter);
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
        } else if (const auto* pr = dyn_cast<PrintStmt>(s.get())) {
            std::vector<const Expr*> items; if (pr->value) items.push_back(pr->value.get()); for (const auto& v : pr->more) items.push_back(v.get());
            for (size_t pi = 0; pi < items.size(); ++pi) {
                const bool last = (pi + 1 == items.size());
                const Expr* v = items[pi];
                auto isStr = [&](const Expr* e, const auto& self) -> bool {
                    if (isa<StringExpr>(e)) return true;
                    if (const auto* vv = dyn_cast<VarExpr>(e)) return !vv->name.empty() && vv->name.back() == Symbols::DOLLARSIGN.first();
                    if (const auto* bb = dyn_cast<BinaryExpr>(e)) return (bb->op == BinaryOp::Add) && (self(bb->lhs.get(), self) || self(bb->rhs.get(), self));
                    return false;
                };
                if (isStr(v, isStr)) {
                    auto sptr = emitExpr(out, v, currLineLabel);
                    std::string fmt = nextTemp(); { std::string ir2 = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")); out << ir2 << Symbols::LF; }
                    { std::string ir3 = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, sptr); out << ir3 << Symbols::LF; }
                } else {
                    // Determine formatting flags for numeric auto formats
                    const bool addNL = last;
                    bool nextStartsWithSpace = false;
                    if (!last && (pi + 1) < items.size()) {
                        if (const auto* ns = dyn_cast<const StringExpr>(items[pi + 1])) {
                            if (!ns->value.empty() && ns->value.front() == ' ') nextStartsWithSpace = true;
                        }
                    }
                    // Constant number? Avoid runtime fcmp in while prints as well
                    if (const auto* cnum = dyn_cast<const NumberExpr>(v)) {
                        const double cv = cnum->value;
                        const bool isIntegral = (std::floor(cv) == cv);
                        if (isIntegral) {
                            std::string fmtI = getFmtIntPtr(out, addNL, nextStartsWithSpace);
                            long long iv = static_cast<long long>(cv);
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, iv); out << ir2 << Symbols::LF; }
                        } else {
                            std::string fmtF = getFmtNumPtr(out, addNL, nextStartsWithSpace);
                            { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {:.6f})", fmtF, cv); out << ir2 << Symbols::LF; }
                        }
                    } else {
                        auto val = emitExpr(out, v, currLineLabel);
                        // Build float and int format pointers
                        std::string fmtF = getFmtNumPtr(out, addNL, nextStartsWithSpace);
                        std::string fmtI = getFmtIntPtr(out, addNL, nextStartsWithSpace);
                        // Integer detection
                        std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", iv, val); out << ir << Symbols::LF; }
                        std::string dv = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", dv, iv); out << ir << Symbols::LF; }
                        std::string isInt = nextTemp(); { std::string ir = std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val); out << ir << Symbols::LF; }
                        std::string intLbl = std::format("{}_wprint_int_{}", currLineLabel, ++localCounter);
                        std::string fltLbl = std::format("{}_wprint_flt_{}", currLineLabel, localCounter);
                        std::string contLbl = std::format("{}_wprint_cont_{}", currLineLabel, localCounter);
                        { std::string ir = std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl); out << ir << Symbols::LF; }
                        out << intLbl << ":" << Symbols::LF;
                        { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, iv); out << ir2 << Symbols::LF; }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        out << fltLbl << ":" << Symbols::LF;
                        { std::string ir2 = std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmtF, val); out << ir2 << Symbols::LF; }
                        { std::string ir = std::format("  br label %{}", contLbl); out << ir << Symbols::LF; }
                        out << contLbl << ":" << Symbols::LF;
                    }
                }
            }
        } else if (const auto* aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
            const auto &dims = arrayDims_[aaset->name];
            long long total = 1; for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); ext = std::max<long long>(ext, 0); total *= ext; }
            std::vector<std::string> idxI64s; idxI64s.reserve(aaset->indices.size());
            std::vector<std::string> bads; bads.reserve(aaset->indices.size());
            for (size_t di = 0; di < aaset->indices.size(); ++di) {
                std::string idxReg = emitExpr(out, aaset->indices[di].get(), currLineLabel);
                std::string idxI64 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i64", idxI64, idxReg); out << ir << Symbols::LF; }
                idxI64s.push_back(idxI64);
                std::string ltBase = nextTemp(); { std::string ir = std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI64, optionBase_); out << ir << Symbols::LF; }
                std::string gtUb = nextTemp(); { std::string ir = std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI64, dims[di]); out << ir << Symbols::LF; }
                std::string bad = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb); out << ir << Symbols::LF; }
                bads.push_back(bad);
            }
            std::string anyBad = bads[0];
            for (size_t i = 1; i < bads.size(); ++i) { std::string nb = nextTemp(); { std::string ir = std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]); out << ir << Symbols::LF; } anyBad = nb; }
            std::string doLbl = std::format("{}_while_arr_ok_{}", currLineLabel, ++localCounter);
            std::string errLbl = std::format("{}_while_arr_err_{}", currLineLabel, localCounter);
            { std::string ir = std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl); out << ir << Symbols::LF; }
            out << errLbl << ":" << Symbols::LF;
            emitErrorDispatch(out, 9, currentLine_, 0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            // Mirror into ERR/ERL variables for runtime bounds errors
            ensureVarAllocated(out, "ERR");
            ensureVarAllocated(out, "ERL");
            { std::string derr = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 9 to double", derr); out << ir << Symbols::LF; } storeNumberToVar(out, "ERR", derr); }
            { std::string dln = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", dln, currentLine_); out << ir << Symbols::LF; } storeNumberToVar(out, "ERL", dln); }
            // Switch emitted above by helper
            out << doLbl << ":" << Symbols::LF;
            std::vector<long long> extents; extents.reserve(dims.size());
            for (size_t di = 0; di < dims.size(); ++di) { long long e = static_cast<long long>(dims[di]) - optionBase_ + 1; e = std::max<long long>(e, 0); extents.push_back(e); }
            std::vector<long long> strides(dims.size(), 1);
            for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) { strides[di] = strides[di + 1] * extents[di + 1]; }
            std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
            for (const auto& ii : idxI64s) { std::string a = nextTemp(); out << std::format("  {} = sub i64 {}, {}", a, ii, optionBase_) << Symbols::LF; adjs.push_back(a); }
            std::string lin = nextTemp(); out << std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]) << Symbols::LF;
            for (size_t di = 1; di < adjs.size(); ++di) { std::string t = nextTemp(); out << std::format("  {} = mul i64 {}, {}", t, adjs[di], strides[di]) << Symbols::LF; std::string s2 = nextTemp(); out << std::format("  {} = add i64 {}, {}", s2, lin, t) << Symbols::LF; lin = s2; }
            if (isStringArrayNameCG(aaset->name)) {
                ensureStringArrayAllocated(out, aaset->name, static_cast<int>(total));
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin) << Symbols::LF;
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                { std::string ir = std::format("  store ptr {}, ptr {}", val, elem); out << ir << Symbols::LF; }
            } else {
                ensureArrayAllocated(out, aaset->name, static_cast<int>(total));
                std::string base = arrayAllocaName_[aaset->name];
                std::string elem = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, total, arrayElemType(aaset->name), base, lin) << Symbols::LF;
                std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
                storeNumberToArrayElem(out, aaset->name, elem, val);
            }
        } else if (const auto* fs = dyn_cast<ForStmt>(s.get())) {
            emitFor(out, fs, currLineLabel, localCounter);
        } else if (const auto* ib = dyn_cast<IfBlockStmt>(s.get())) {
            emitIfBlock(out, ib, currLineLabel, localCounter);
        } else if (const auto* ws2 = dyn_cast<WhileStmt>(s.get())) {
            emitWhile(out, ws2, currLineLabel, localCounter);
        } else if (isa<ReturnStmt>(s.get())) {
            std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF;
        } else if (isa<EndStmt>(s.get())) {
            std::string ir = std::format("  br label %exit"); out << ir << Symbols::LF;
        } else if (const auto* gt = dyn_cast<GotoStmt>(s.get())) {
            std::string ir = std::format("  br label %{}", lineLabelName(gt->targetLine)); out << ir << Symbols::LF;
        } else if (const auto* gs = dyn_cast<GosubStmt>(s.get())) {
            std::string contLbl = std::format("{}_gosub_cont{}", currLineLabel, ++localCounter);
            std::string entryLbl = std::format("{}_gosub_entry{}", currLineLabel, localCounter);
            out << std::format("  br label %{}", entryLbl) << Symbols::LF;
            emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
            out << std::format("{}:", contLbl) << Symbols::LF;
        } else if (const auto* og = dyn_cast<OnGotoStmt>(s.get())) {
            std::string idx = emitExpr(out, og->index.get(), currLineLabel);
            std::string idxi32 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", idxi32, idx); out << ir << Symbols::LF; }
            std::string contLbl = std::format("{}_on_cont_{}", currLineLabel, ++localCounter);
            out << std::format("  switch i32 {}, label %{} [", idxi32, contLbl) << Symbols::LF;
            for (size_t i = 0; i < og->targets.size(); ++i) out << std::format("    i32 {}, label %{}", i+1, lineLabelName(og->targets[i])) << Symbols::LF;
            out << "  ]" << Symbols::LF;
            out << std::format("{}:", contLbl) << Symbols::LF;
        } else if (const auto* ogs = dyn_cast<OnGosubStmt>(s.get())) {
            std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
            std::string idxi32 = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", idxi32, idx); out << ir << Symbols::LF; }
            std::string contLbl = std::format("{}_on_gs_cont_{}", currLineLabel, ++localCounter);
            std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
            for (size_t i = 0; i < ogs->targets.size(); ++i) entryLbls.push_back(std::format("{}_on_gs_entry_{}_{}", currLineLabel, localCounter, i+1));
            out << std::format("  switch i32 {}, label %{} [", idxi32, contLbl) << Symbols::LF;
            for (size_t i = 0; i < ogs->targets.size(); ++i) out << std::format("    i32 {}, label %{}", i+1, entryLbls[i]) << Symbols::LF;
            out << "  ]" << Symbols::LF;
            for (size_t i = 0; i < ogs->targets.size(); ++i) emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
            out << std::format("{}:", contLbl) << Symbols::LF;
        } else if (isa<StopStmt>(s.get())) {
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_); out << ir << Symbols::LF; }
            out << std::format("  br label %{}", endLbl) << Symbols::LF;
        } else if (isa<SystemStmt>(s.get())) {
            out << std::format("  br label %{}", endLbl) << Symbols::LF;
        } else if (const auto* ins = dyn_cast<InputStmt>(s.get())) {
            if (ins->promptLiteral || ins->promptVar) {
                std::string pstr;
                if (ins->promptLiteral) {
                    if (strLiteralId_.contains(*ins->promptLiteral)) {
                        int id = strLiteralId_[*ins->promptLiteral];
                        pstr = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", pstr, globalStringName(id)); out << ir << Symbols::LF; }
                    }
                } else {
                    ensureVarAllocated(out, *ins->promptVar);
                    pstr = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", pstr, varAllocaName_[*ins->promptVar]); out << ir << Symbols::LF; }
                    std::string safe = nextTemp(); { std::string ir = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, pstr); out << ir << Symbols::LF; } pstr = safe;
                }
                if (!pstr.empty()) {
                    std::string fmtS = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_str_sp, i64 0", fmtS); out << ir << Symbols::LF; }
                    { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmtS, pstr); out << ir << Symbols::LF; }
                }
            }
            for (const auto& vname : ins->variables) {
                ensureVarAllocated(out, vname);
                std::string fmt = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; }
                std::string tmp = nextTemp(); { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
                { std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp); out << ir2 << Symbols::LF; }
                std::string dv = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
                storeNumberToVar(out, vname, dv);
            }
        } else if (auto* col = dyn_cast<ColorStmt>(s.get())) {
            auto emitColor = [&](const std::unique_ptr<Expr>& e, bool isFg){
                if (!e) return;
                std::string val = emitExpr(out, e.get(), "");
                std::string i32v = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", i32v, val); out << ir << Symbols::LF; }
                std::string masked = nextTemp(); { std::string ir = std::format("  {} = and i32 {}, 15", masked, i32v); out << ir << Symbols::LF; }
                std::string idx64 = nextTemp(); { std::string ir = std::format("  {} = sext i32 {} to i64", idx64, masked); out << ir << Symbols::LF; }
                std::string p = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [16 x i32], ptr {}, i64 0, i64 {}", p, (isFg?"@.sgr_fg_tbl":"@.sgr_bg_tbl"), idx64); out << ir << Symbols::LF; }
                std::string code = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr {}", code, p); out << ir << Symbols::LF; }
                std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [7 x i8], ptr @.fmt_sgr, i64 0, i64 0", fmt); out << ir << Symbols::LF; }
                { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 27, i32 {})", fmt, code); out << ir << Symbols::LF; }
            };
            emitColor(col->fg, true);
            emitColor(col->bg, false);
        } else {
            throw CodeGenError("Unsupported statement in WHILE body");
        }
    }
    // Jump back to condition
    { std::string ir = std::format("  br label %{}", condLbl); out << ir << Symbols::LF; }
    // End label
    out << endLbl << ":" << Symbols::LF;
}

// NOLINTEND(readability-function-cognitive-complexity,readability-function-size,readability-identifier-length,readability-avoid-nested-conditional-operator,readability-braces-around-statements,misc-const-correctness)
} // namespace gwbasic
