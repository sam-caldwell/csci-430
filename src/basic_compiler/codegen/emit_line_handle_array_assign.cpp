// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <vector>

namespace gwbasic {

// Array element assignment A(i[,j...]) = expr
// NOLINTNEXTLINE(readability-function-size)
void CodeGenerator::emitLineHandleArrayAssign(std::ostringstream &out,
                                              const Stmt *stmt,
                                              int stmtIndex,
                                              const std::string &currLineLabel,
                                              int &localCounter) {
    const auto *aaset = dyn_cast<ArrayAssignStmt>(stmt);
    if (!aaset) { return; }
    const auto &dims = arrayDims_[aaset->name];
    long long total = 1; for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); if (ext < 0) ext = 0; total *= ext; }
    std::vector<std::string> idxI64s; idxI64s.reserve(aaset->indices.size());
    std::vector<std::string> bads; bads.reserve(aaset->indices.size());
    for (size_t di = 0; di < aaset->indices.size(); ++di) {
        const std::string idxD = emitExpr(out, aaset->indices[di].get(), "");
        const std::string idxI = nextTemp(); out << std::format("  {} = fptosi double {} to i64", idxI, idxD) << Symbols::LF;
        idxI64s.push_back(idxI);
        const std::string ltBase = nextTemp(); out << std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI, optionBase_) << Symbols::LF;
        const std::string gtUb = nextTemp(); out << std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI, dims[di]) << Symbols::LF;
        const std::string bad = nextTemp(); out << std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb) << Symbols::LF;
        bads.push_back(bad);
    }
    std::string anyBad = bads[0];
    for (size_t i = 1; i < bads.size(); ++i) {
        const std::string newBad = nextTemp();
        out << std::format("  {} = or i1 {}, {}", newBad, anyBad, bads[i]) << Symbols::LF;
        anyBad = newBad;
    }
    const std::string doLbl = std::format("{}_arr_ok_{}", currLineLabel, ++localCounter);
    const std::string errLbl = std::format("{}_arr_err_{}", currLineLabel, localCounter);
    out << std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl) << Symbols::LF;
    out << errLbl << ":" << Symbols::LF;
    out << "  store i32 9, ptr @gwb_err_code" << Symbols::LF;
    out << std::format("  store i32 {}, ptr @gwb_err_line", currentLine_) << Symbols::LF;
    out << std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_) << Symbols::LF;
    out << std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex) << Symbols::LF;
    out << "  store i1 true, ptr @gwb_in_handler" << Symbols::LF;
    ensureVarAllocated(out, "ERR");
    ensureVarAllocated(out, "ERL");
    { const std::string derr = nextTemp(); out << std::format("  {} = sitofp i32 9 to double", derr) << Symbols::LF; storeNumberToVar(out, "ERR", derr); }
    { const std::string dln = nextTemp(); out << std::format("  {} = sitofp i32 {} to double", dln, currentLine_) << Symbols::LF; storeNumberToVar(out, "ERL", dln); }
    {
        const std::string trap = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF;
        out << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
        for (int lnum : lineNumbers_) { out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF; }
        out << "  ]" << Symbols::LF;
    }
    out << doLbl << ":" << Symbols::LF;
    std::vector<long long> extents; extents.reserve(dims.size());
    for (size_t di = 0; di < dims.size(); ++di) {
        long long extent = static_cast<long long>(dims[di]) - optionBase_ + 1;
        if (extent < 0) {
            extent = 0;
        }
        extents.push_back(extent);
    }
    std::vector<long long> strides(dims.size(), 1);
    for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) { strides[di] = strides[di + 1] * extents[di + 1]; }
    std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
    for (const auto &ii : idxI64s) { const std::string a = nextTemp(); out << std::format("  {} = sub i64 {}, {}", a, ii, optionBase_) << Symbols::LF; adjs.push_back(a); }
    std::string lin = nextTemp(); out << std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]) << Symbols::LF;
    for (size_t di = 1; di < adjs.size(); ++di) {
        const std::string tmpProd = nextTemp();
        out << std::format("  {} = mul i64 {}, {}", tmpProd, adjs[di], strides[di]) << Symbols::LF;
        const std::string sumTmp = nextTemp();
        out << std::format("  {} = add i64 {}, {}", sumTmp, lin, tmpProd) << Symbols::LF;
        lin = sumTmp;
    }
    if (isStringArrayNameCG(aaset->name)) {
        ensureStringArrayAllocated(out, aaset->name, static_cast<int>(total));
        const std::string base = arrayAllocaName_[aaset->name];
        const std::string elem = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin) << Symbols::LF;
        const std::string val = emitExpr(out, aaset->value.get(), "");
        out << std::format("  store ptr {}, ptr {}", val, elem) << Symbols::LF;
    } else {
        ensureArrayAllocated(out, aaset->name, static_cast<int>(total));
        const std::string base = arrayAllocaName_[aaset->name];
        const std::string elem = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, total, arrayElemType(aaset->name), base, lin) << Symbols::LF;
        const std::string val = emitExpr(out, aaset->value.get(), "");
        storeNumberToArrayElem(out, aaset->name, elem, val);
    }
    (void)currLineLabel;
}

} // namespace gwbasic
