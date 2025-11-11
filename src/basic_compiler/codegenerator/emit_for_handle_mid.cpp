// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <algorithm>
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <vector>

namespace gwbasic {

/*
 * Function: emitForHandleMidAssign
 * Summary: Emit IR for MID$ assignment inside a FOR body (scalar or array).
 * Parameters:
 *  - out: IR output stream to append to.
 *  - mid: Parsed MidAssignStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-function-size)
void CodeGenerator::emitForHandleMidAssign(std::ostringstream& out, const MidAssignStmt* mid, const std::string& currLineLabel, int& localCounter) {
    static constexpr int kErrArrayBounds = 9;
    std::string dest;
    if (!mid->indices.empty()) {
        const auto &dims = arrayDims_[mid->name];
        long long total = 1;
        for (const int upper_bound : dims) {
            long long ext = (static_cast<long long>(upper_bound) - optionBase_ + 1);
            ext = std::max<long long>(ext, 0);
            total *= ext;
        }
        ensureStringArrayAllocated(out, mid->name, static_cast<int>(total));
        const std::string base = arrayAllocaName_[mid->name];
        std::vector<std::string> idxI64s; idxI64s.reserve(mid->indices.size());
        std::vector<std::string> bads; bads.reserve(mid->indices.size());
        for (size_t di = 0; di < mid->indices.size(); ++di) {
            const std::string idxReg = emitExpr(out, mid->indices[di].get(), currLineLabel);
            const std::string idxI64 = nextTemp();
            idxI64s.push_back(idxI64);
            const std::string ltBase = nextTemp();
            const std::string gtUb = nextTemp();
            const std::string bad = nextTemp();
            bads.push_back(bad);
            out << std::format("  {} = fptosi double {} to i64", idxI64, idxReg) << Symbols::LF
                << std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI64, optionBase_) << Symbols::LF
                << std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI64, dims[di]) << Symbols::LF
                << std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb) << Symbols::LF;
        }
        std::string anyBad = bads[0];
        for (size_t i = 1; i < bads.size(); ++i) {
            const std::string next_bad = nextTemp();
            out << std::format("  {} = or i1 {}, {}", next_bad, anyBad, bads[i]) << Symbols::LF;
            anyBad = next_bad;
        }
        const std::string doLbl = std::format("{}_mid_ok_{}", currLineLabel, ++localCounter);
        const std::string errLbl = std::format("{}_mid_err_{}", currLineLabel, localCounter);
        emitErrorDispatch(out, kErrArrayBounds, currentLine_, /*stmtIndex=*/0);
        const std::string lin = emitLinearIndex(out, idxI64s, dims);
        const std::string elem = nextTemp();
        const std::string storePtr = nextTemp();
        out << std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl) << Symbols::LF
            << errLbl << ":" << Symbols::LF
            << doLbl << ":" << Symbols::LF
            << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin) << Symbols::LF
            << std::format("  {} = load ptr, ptr {}", storePtr, elem) << Symbols::LF;
        dest = storePtr;
    } else {
        ensureVarAllocated(out, mid->name);
        dest = varAllocaName_[mid->name];
    }
    const std::string off = nextTemp(); out << std::format("  {} = fptosi double {} to i64", off, emitExpr(out, mid->start.get(), currLineLabel)) << Symbols::LF;
    const std::string dlen = nextTemp(); out << std::format("  {} = call i64 @strlen(ptr {})", dlen, dest) << Symbols::LF;
    const std::string len_i64 = nextTemp();
    if (mid->len) {
        out << std::format("  {} = fptosi double {} to i64", len_i64, emitExpr(out, mid->len.get(), currLineLabel)) << Symbols::LF; // NOLINT(bugprone-branch-clone)
    } else {
        out << std::format("  {} = call i64 @strlen(ptr {})", len_i64, emitExpr(out, mid->value.get(), currLineLabel)) << Symbols::LF; // NOLINT(bugprone-branch-clone)
    }
    const std::string src = nextTemp();
    const std::string negOff = nextTemp();
    const std::string geLen = nextTemp();
    const std::string bad = nextTemp();
    const std::string doLbl2 = std::format("{}_mid_do_{}", currLineLabel, ++localCounter);
    const std::string endLbl2b = std::format("{}_mid_end_{}", currLineLabel, localCounter);
    const std::string avail = nextTemp();
    const std::string n_lt_avail = nextTemp();
    const std::string min1 = nextTemp();
    const std::string min1_lt_str = nextTemp();
    const std::string min2 = nextTemp();
    const std::string dst_ptr = nextTemp();

    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", src, emitExpr(out, mid->value.get(), currLineLabel)) << Symbols::LF
        << std::format("  {} = icmp slt i64 {}, 0", negOff, off) << Symbols::LF
        << std::format("  {} = icmp sge i64 {}, {}", geLen, off, dlen) << Symbols::LF
        << std::format("  {} = or i1 {}, {}", bad, negOff, geLen) << Symbols::LF
        << std::format("  br i1 {}, label %{}, label %{}", bad, endLbl2b, doLbl2) << Symbols::LF
        << doLbl2 << ":" << Symbols::LF
        << std::format("  {} = sub i64 {}, {}", avail, dlen, off) << Symbols::LF
        << std::format("  {} = icmp slt i64 {}, {}", n_lt_avail, len_i64, avail) << Symbols::LF
        << std::format("  {} = select i1 {}, i64 {}, i64 {}", min1, n_lt_avail, len_i64, avail) << Symbols::LF
        << std::format("  {} = icmp slt i64 {}, {}", min1_lt_str, min1, dlen) << Symbols::LF
        << std::format("  {} = select i1 {}, i64 {}, i64 {}", min2, min1_lt_str, min1, dlen) << Symbols::LF
        << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", dst_ptr, dest, off) << Symbols::LF
        << std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", dst_ptr, src, min2) << Symbols::LF
        << std::format("  br label %{}", endLbl2b) << Symbols::LF
        << endLbl2b << ":" << Symbols::LF;
}

} // namespace gwbasic
