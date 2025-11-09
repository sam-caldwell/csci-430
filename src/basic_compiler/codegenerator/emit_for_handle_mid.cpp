// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForHandleMidAssign
 * Purpose: Emit IR for MID$ assignment inside a FOR body (scalar or array).
 */
void CodeGenerator::emitForHandleMidAssign(std::ostringstream& out, const MidAssignStmt* mid, const std::string& currLineLabel, int& localCounter) {
    static constexpr int kErrArrayBounds = 9;
    std::string dest;
    if (!mid->indices.empty()) {
        const auto &dims = arrayDims_[mid->name];
        long long total = 1;
        for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); if (ext < 0) ext = 0; total *= ext; }
        ensureStringArrayAllocated(out, mid->name, static_cast<int>(total));
        std::string base = arrayAllocaName_[mid->name];
        std::vector<std::string> idxI64s; idxI64s.reserve(mid->indices.size());
        std::vector<std::string> bads; bads.reserve(mid->indices.size());
        for (size_t di = 0; di < mid->indices.size(); ++di) {
            std::string idxReg = emitExpr(out, mid->indices[di].get(), currLineLabel);
            std::string idxI64 = nextTemp();
            idxI64s.push_back(idxI64);
            std::string ltBase = nextTemp();
            std::string gtUb = nextTemp();
            std::string bad = nextTemp();
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
        std::string doLbl = std::format("{}_mid_ok_{}", currLineLabel, ++localCounter);
        std::string errLbl = std::format("{}_mid_err_{}", currLineLabel, localCounter);
        emitErrorDispatch(out, kErrArrayBounds, currentLine_, /*stmtIndex=*/0);
        std::string lin = emitLinearIndex(out, idxI64s, dims);
        std::string elem = nextTemp();
        auto storePtr = nextTemp();
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
    auto off = nextTemp(); out << std::format("  {} = fptosi double {} to i64", off, emitExpr(out, mid->start.get(), currLineLabel)) << Symbols::LF;
    auto dlen = nextTemp(); out << std::format("  {} = call i64 @strlen(ptr {})", dlen, dest) << Symbols::LF;
    auto n = nextTemp();
    if (mid->len) {
        out << std::format("  {} = fptosi double {} to i64", n, emitExpr(out, mid->len.get(), currLineLabel)) << Symbols::LF;
    } else {
        out << std::format("  {} = call i64 @strlen(ptr {})", n, emitExpr(out, mid->value.get(), currLineLabel)) << Symbols::LF;
    }
    auto src = nextTemp();
    auto negOff = nextTemp();
    auto geLen = nextTemp();
    auto bad = nextTemp();
    auto doLbl = std::format("{}_mid_do_{}", currLineLabel, ++localCounter);
    std::string endLbl2b = std::format("{}_mid_end_{}", currLineLabel, localCounter);
    std::string avail = nextTemp();
    std::string n_lt_av = nextTemp();
    std::string m1 = nextTemp();
    std::string m1_lt_s = nextTemp();
    std::string m2 = nextTemp();
    std::string dst = nextTemp();

    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", src, emitExpr(out, mid->value.get(), currLineLabel)) << Symbols::LF
        << std::format("  {} = icmp slt i64 {}, 0", negOff, off) << Symbols::LF
        << std::format("  {} = icmp sge i64 {}, {}", geLen, off, dlen) << Symbols::LF
        << std::format("  {} = or i1 {}, {}", bad, negOff, geLen) << Symbols::LF
        << std::format("  br i1 {}, label %{}, label %{}", bad, endLbl2b, doLbl) << Symbols::LF
        << doLbl << ":" << Symbols::LF
        << std::format("  {} = sub i64 {}, {}", avail, dlen, off) << Symbols::LF
        << std::format("  {} = icmp slt i64 {}, {}", n_lt_av, n, avail) << Symbols::LF
        << std::format("  {} = select i1 {}, i64 {}, i64 {}", m1, n_lt_av, n, avail) << Symbols::LF
        << std::format("  {} = icmp slt i64 {}, {}", m1_lt_s, m1, dlen) << Symbols::LF
        << std::format("  {} = select i1 {}, i64 {}, i64 {}", m2, m1_lt_s, m1, dlen) << Symbols::LF
        << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", dst, dest, off) << Symbols::LF
        << std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", dst, src, m2) << Symbols::LF
        << std::format("  br label %{}", endLbl2b) << Symbols::LF
        << endLbl2b << ":" << Symbols::LF;
}

} // namespace gwbasic

