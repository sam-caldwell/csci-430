// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include <sstream>
#include <format>

namespace gwbasic {

/*
 * Function: emitForHandleArrayAssign
 * Summary: Emit IR for array element assignment inside a FOR body.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - aaset: Parsed ArrayAssignStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForHandleArrayAssign(std::ostringstream& out, const ArrayAssignStmt* aaset, const std::string& currLineLabel, int& localCounter) {
    static constexpr int kErrArrayBounds = 9;
    const auto &dims = arrayDims_[aaset->name];
    long long total = 1;
    for (int ub : dims) {
        long long ext = (static_cast<long long>(ub) - optionBase_ + 1);
        if (ext < 0)
            ext = 0;
        total *= ext;
    }
    std::vector<std::string> idxI64s; idxI64s.reserve(aaset->indices.size());
    std::vector<std::string> bads; bads.reserve(aaset->indices.size());
    for (size_t di = 0; di < aaset->indices.size(); ++di) {
        std::string idxReg = emitExpr(out, aaset->indices[di].get(), currLineLabel);
        std::string idxI64 = nextTemp();
        std::string ltBase = nextTemp();
        std::string gtUb = nextTemp();
        std::string bad = nextTemp();
        idxI64s.push_back(idxI64);
        bads.push_back(bad);
        out << std::format("  {} = fptosi double {} to i64", idxI64, idxReg) << Symbols::LF
            << std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI64, optionBase_) << Symbols::LF
            << std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI64, dims[di]) << Symbols::LF
            << std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb) << Symbols::LF;
    }
    std::string anyBad = bads[0];
    for (size_t i = 1; i < bads.size(); ++i) {
        std::string nb = nextTemp();
        out << std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]) << Symbols::LF;
        anyBad = nb;
    }
    std::string doLbl = std::format("{}_for_arr_ok_{}", currLineLabel, ++localCounter);
    std::string errLbl = std::format("{}_for_arr_err_{}", currLineLabel, localCounter);
    out << std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl) << Symbols::LF;
    out << errLbl << ":" << Symbols::LF;
    emitErrorDispatch(out, kErrArrayBounds, currentLine_, /*stmtIndex=*/0);
    out << doLbl << ":" << Symbols::LF;
    std::string lin = emitLinearIndex(out, idxI64s, dims);
    if (isStringArrayNameCG(aaset->name)) {
        ensureStringArrayAllocated(out, aaset->name, static_cast<int>(total));
        std::string base = arrayAllocaName_[aaset->name];
        std::string elem = nextTemp();
        std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
        out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin)
            << Symbols::LF
            << std::format("  store ptr {}, ptr {}", val, elem)
            << Symbols::LF;
    } else {
        ensureArrayAllocated(out, aaset->name, static_cast<int>(total));
        std::string base = arrayAllocaName_[aaset->name];
        std::string elem = nextTemp();
        std::string val = emitExpr(out, aaset->value.get(), currLineLabel);
        storeNumberToArrayElem(out, aaset->name, elem, val);
        out << std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, total, arrayElemType(aaset->name), base, lin)
            << Symbols::LF;
    }
}

} // namespace gwbasic
