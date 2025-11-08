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
            std::string idxI64 = nextTemp(); out << std::format("  {} = fptosi double {} to i64", idxI64, idxReg) << Symbols::LF;
            idxI64s.push_back(idxI64);
            std::string ltBase = nextTemp(); out << std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI64, optionBase_) << Symbols::LF;
            std::string gtUb = nextTemp(); out << std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI64, dims[di]) << Symbols::LF;
            std::string bad = nextTemp(); out << std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb) << Symbols::LF;
            bads.push_back(bad);
        }
        std::string anyBad = bads[0];
        for (size_t i = 1; i < bads.size(); ++i) { std::string nb = nextTemp(); out << std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]) << Symbols::LF; anyBad = nb; }
        std::string doLbl = currLineLabel + std::string("_mid_ok_") + std::to_string(++localCounter);
        std::string errLbl = currLineLabel + std::string("_mid_err_") + std::to_string(localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl) << Symbols::LF;
        out << errLbl << ":" << Symbols::LF;
        emitErrorDispatch(out, kErrArrayBounds, currentLine_, /*stmtIndex=*/0);
        out << doLbl << ":" << Symbols::LF;
        std::string lin = emitLinearIndex(out, idxI64s, dims);
        std::string elem = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin) << Symbols::LF;
        auto storePtr = nextTemp(); out << std::format("  {} = load ptr, ptr {}", storePtr, elem) << Symbols::LF;
        dest = storePtr;
    } else {
        ensureVarAllocated(out, mid->name);
        dest = varAllocaName_[mid->name];
    }
    auto off = nextTemp(); out << std::format("  {} = fptosi double {} to i64", off, emitExpr(out, mid->start.get(), currLineLabel)) << Symbols::LF;
    auto dlen = nextTemp(); out << std::format("  {} = call i64 @strlen(ptr {})", dlen, dest) << Symbols::LF;
    auto n = nextTemp();
    if (mid->len) {
        auto lenD = emitExpr(out, mid->len.get(), currLineLabel);
        out << std::format("  {} = fptosi double {} to i64", n, lenD) << Symbols::LF;
    } else {
        out << std::format("  {} = call i64 @strlen(ptr {})", n, emitExpr(out, mid->value.get(), currLineLabel)) << Symbols::LF;
    }
    auto src = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", src, emitExpr(out, mid->value.get(), currLineLabel)) << Symbols::LF;
    auto negOff = nextTemp(); out << std::format("  {} = icmp slt i64 {}, 0", negOff, off) << Symbols::LF;
    auto geLen = nextTemp(); out << std::format("  {} = icmp sge i64 {}, {}", geLen, off, dlen) << Symbols::LF;
    auto bad = nextTemp(); out << std::format("  {} = or i1 {}, {}", bad, negOff, geLen) << Symbols::LF;
    auto doLbl = currLineLabel + std::string("_mid_do_") + std::to_string(++localCounter);
    std::string endLbl2b = currLineLabel + std::string("_mid_end_") + std::to_string(localCounter);
    out << std::format("  br i1 {}, label %{}, label %{}", bad, endLbl2b, doLbl) << Symbols::LF;
    out << doLbl << ":" << Symbols::LF;
    std::string avail = nextTemp(); out << std::format("  {} = sub i64 {}, {}", avail, dlen, off) << Symbols::LF;
    std::string n_lt_av = nextTemp(); out << std::format("  {} = icmp slt i64 {}, {}", n_lt_av, n, avail) << Symbols::LF;
    std::string m1 = nextTemp(); out << std::format("  {} = select i1 {}, i64 {}, i64 {}", m1, n_lt_av, n, avail) << Symbols::LF;
    std::string m1_lt_s = nextTemp(); out << std::format("  {} = icmp slt i64 {}, {}", m1_lt_s, m1, dlen) << Symbols::LF;
    std::string m2 = nextTemp(); out << std::format("  {} = select i1 {}, i64 {}, i64 {}", m2, m1_lt_s, m1, dlen) << Symbols::LF;
    std::string dst = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", dst, dest, off) << Symbols::LF;
    out << std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", dst, src, m2) << Symbols::LF;
    out << std::format("  br label %{}", endLbl2b) << Symbols::LF;
    out << endLbl2b << ":" << Symbols::LF;
}

} // namespace gwbasic

