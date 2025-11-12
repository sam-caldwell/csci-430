// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/SwapStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>
#include <string>
#include <vector>

namespace gwbasic {

// SWAP left, right — supports scalars and array elements (numeric and string)
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity,readability-identifier-length)
void CodeGenerator::emitLineHandleSwap(std::ostringstream &out,
                                       const Stmt *stmt,
                                       int stmtIndex,
                                       const std::string &currLineLabel,
                                       int &localCounter) {
    const auto *sw = dyn_cast<SwapStmt>(stmt);
    if (!sw) { return; }
    struct RefInfo {
        bool isString{false};
        bool isArray{false};
        std::string name;
        std::string ptr;  // ptr to storage (var alloca or array element)
        long long total{0};
    } L, R;
    auto computeRef = [&](const ReadTarget &t, RefInfo &outInfo) {
        outInfo.name = t.name;
        outInfo.isArray = !t.indices.empty();
        outInfo.isString = isStringVarNameCG(t.name);
        if (!outInfo.isArray) {
            ensureVarAllocated(out, t.name);
            outInfo.ptr = varAllocaName_[t.name];
        } else {
            const auto &dims = arrayDims_[t.name];
            long long total = 1; for (int ub : dims) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); if (ext < 0) ext = 0; total *= ext; }
            outInfo.total = total;
            std::vector<std::string> idxI64s; idxI64s.reserve(t.indices.size());
            std::vector<std::string> bads; bads.reserve(t.indices.size());
            for (size_t di = 0; di < t.indices.size(); ++di) {
                const std::string idxD = emitExpr(out, t.indices[di].get(), "");
                const std::string idxI = nextTemp();
                idxI64s.push_back(idxI);
                const std::string ltBase = nextTemp();
                const std::string gtUb = nextTemp();
                const std::string bad = nextTemp();
                bads.push_back(bad);
                out << std::format("  {} = fptosi double {} to i64", idxI, idxD) << Symbols::LF
                    << std::format("  {} = icmp slt i64 {}, {}", ltBase, idxI, optionBase_) << Symbols::LF
                    << std::format("  {} = icmp sgt i64 {}, {}", gtUb, idxI, dims[di]) << Symbols::LF
                    << std::format("  {} = or i1 {}, {}", bad, ltBase, gtUb) << Symbols::LF;
            }
            std::string anyBad = bads[0];
            for (size_t i = 1; i < bads.size(); ++i) {
                const std::string nb = nextTemp();
                out << std::format("  {} = or i1 {}, {}", nb, anyBad, bads[i]) << Symbols::LF;
                anyBad = nb;
            }
            const std::string doLbl = std::format("{}_swap_ok_{}", currLineLabel, ++localCounter);
            const std::string errLbl = std::format("{}_swap_err_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl, doLbl) << Symbols::LF
                << errLbl << ":" << Symbols::LF
                << "  store i32 9, ptr @gwb_err_code" << Symbols::LF
                << std::format("  store i32 {}, ptr @gwb_err_line", currentLine_) << Symbols::LF
                << std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_) << Symbols::LF
                << std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex) << Symbols::LF
                << "  store i1 true, ptr @gwb_in_handler" << Symbols::LF;
            ensureVarAllocated(out, "ERR");
            ensureVarAllocated(out, "ERL");
            {
                const std::string derr = nextTemp();
                out << std::format("  {} = sitofp i32 9 to double", derr) << Symbols::LF;
                storeNumberToVar(out, "ERR", derr);
            }
            {
                const std::string dln = nextTemp();
                out << std::format("  {} = sitofp i32 {} to double", dln, currentLine_) << Symbols::LF;
                storeNumberToVar(out, "ERL", dln);
            }
            {
                const std::string trap = nextTemp();
                out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF
                    << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
                for (int lnum : lineNumbers_) {
                    out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF;
                }
                out << "  ]" << Symbols::LF;
            }
            out << doLbl << ":" << Symbols::LF;
            std::vector<long long> extents; extents.reserve(dims.size());
            for (size_t di = 0; di < dims.size(); ++di) {
                long long e = static_cast<long long>(dims[di]) - optionBase_ + 1;
                if (e < 0)
                    e = 0;
                extents.push_back(e);
            }
            std::vector<long long> strides(dims.size(), 1);
            for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) {
                strides[di] = strides[di + 1] * extents[di + 1];
            }
            std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
            for (const auto &ii : idxI64s) {
                const std::string a = nextTemp();
                out << std::format("  {} = sub i64 {}, {}", a, ii, optionBase_) << Symbols::LF;
                adjs.push_back(a);
            }
            std::string lin = nextTemp();
            out << std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]) << Symbols::LF;
            for (size_t di = 1; di < adjs.size(); ++di) {
                const std::string t2 = nextTemp();
                const std::string s2 = nextTemp();
                out << std::format("  {} = mul i64 {}, {}", t2, adjs[di], strides[di]) << Symbols::LF
                    << std::format("  {} = add i64 {}, {}", s2, lin, t2) << Symbols::LF;
                lin = s2;
            }
            if (isStringArrayNameCG(t.name)) {
                ensureStringArrayAllocated(out, t.name, static_cast<int>(total));
                const std::string base = arrayAllocaName_[t.name];
                const std::string elem = nextTemp();
                out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, base, lin) << Symbols::LF;
                outInfo.ptr = elem;
            } else {
                ensureArrayAllocated(out, t.name, static_cast<int>(total));
                const std::string base = arrayAllocaName_[t.name];
                const std::string elem = nextTemp();
                out << std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, total, arrayElemType(t.name), base, lin) << Symbols::LF;
                outInfo.ptr = elem;
            }
        }
    };
    computeRef(sw->left, L);
    computeRef(sw->right, R);
    if (L.isString) {
        const std::string leftValPtr = nextTemp();
        out << std::format("  {} = load ptr, ptr {}", leftValPtr, L.ptr) << Symbols::LF;
        const std::string rightValPtr = nextTemp();
        out << std::format("  {} = load ptr, ptr {}", rightValPtr, R.ptr) << Symbols::LF;
        out << std::format("  store ptr {}, ptr {}", rightValPtr, L.ptr) << Symbols::LF;
        out << std::format("  store ptr {}, ptr {}", leftValPtr, R.ptr) << Symbols::LF;
    } else {
        auto loadVarAsDouble = [&](const std::string &name, const std::string &ptr) -> std::string {
            switch (numKindOf(name)) {
                case NumKind::Int16: {
                    const std::string val = nextTemp(); out << std::format("  {} = load i16, ptr {}", val, ptr) << Symbols::LF;
                    const std::string val32 = nextTemp(); out << std::format("  {} = sext i16 {} to i32", val32, val) << Symbols::LF;
                    const std::string dbl = nextTemp(); out << std::format("  {} = sitofp i32 {} to double", dbl, val32) << Symbols::LF; return dbl;
                }
                case NumKind::Long32: {
                    const std::string val = nextTemp(); out << std::format("  {} = load i32, ptr {}", val, ptr) << Symbols::LF;
                    const std::string dbl = nextTemp(); out << std::format("  {} = sitofp i32 {} to double", dbl, val) << Symbols::LF; return dbl;
                }
                case NumKind::Single: {
                    const std::string val = nextTemp(); out << std::format("  {} = load float, ptr {}", val, ptr) << Symbols::LF;
                    const std::string dbl = nextTemp(); out << std::format("  {} = fpext float {} to double", dbl, val) << Symbols::LF; return dbl;
                }
                case NumKind::Double: default: {
                    const std::string dbl = nextTemp(); out << std::format("  {} = load double, ptr {}", dbl, ptr) << Symbols::LF; return dbl;
                }
            }
        };
        const std::string leftVal = loadVarAsDouble(L.name, L.ptr);
        const std::string rightVal = loadVarAsDouble(R.name, R.ptr);
        if (!L.isArray) {
            storeNumberToVar(out, L.name, rightVal); }
        else {
            storeNumberToArrayElem(out, L.name, L.ptr, rightVal); }
        if (!R.isArray) {
            storeNumberToVar(out, R.name, leftVal); }
        else {
            storeNumberToArrayElem(out, R.name, R.ptr, leftVal); }
    }
}

} // namespace gwbasic
