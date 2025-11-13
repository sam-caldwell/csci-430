// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/RestoreStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include <algorithm>
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <vector>

namespace gwbasic {

// READ / RESTORE / DATA
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity,readability-identifier-length)
void CodeGenerator::emitLineHandleReadRestoreData(std::ostringstream &out,
                                                  const Stmt *stmt,
                                                  int stmtIndex,
                                                  const std::string &currLineLabel,
                                                  int &localCounter) {
    if (isa<DataStmt>(stmt)) {
        log() << "line " << currentLine_ << " DataStmt (no-op)" << Symbols::LF;
        return;
    }
    if (isa<RestoreStmt>(stmt)) {
        const std::string storeIr = std::format("  store i32 0, ptr @gwb_data_idx");
        out << storeIr << Symbols::LF;
        log() << "line " << currentLine_ << " RestoreStmt data_idx -> " << storeIr << Symbols::LF;
        return;
    }
    const auto *readStmt = dyn_cast<ReadStmt>(stmt);
    if (readStmt == nullptr) { return; }
    for (const auto &target : readStmt->targets) {
        const std::string idx = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_data_idx", idx) << Symbols::LF;
        const size_t dataCount = dataLiteralIds_.size();
        const std::string inBounds = nextTemp(); out << std::format("  {} = icmp ult i32 {}, {}", inBounds, idx, static_cast<int>(dataCount)) << Symbols::LF;
        const std::string okLbl = std::format("{}_read_ok_{}", currLineLabel, ++localCounter);
        const std::string errLbl = std::format("{}_read_err_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", inBounds, okLbl, errLbl) << Symbols::LF;
        out << errLbl << ":" << Symbols::LF;
        out << "  store i32 9, ptr @gwb_err_code" << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_err_line", currentLine_) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex) << Symbols::LF;
        out << "  store i1 true, ptr @gwb_in_handler" << Symbols::LF;
        {
            const std::string trap = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF;
            out << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
            for (int lnum : lineNumbers_) { out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF; }
            out << "  ]" << Symbols::LF;
        }
        out << okLbl << ":" << Symbols::LF;
        const std::string idx64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", idx64, idx) << Symbols::LF;
        const std::string elemPtr = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr @gwb_data, i64 0, i64 {}", elemPtr, dataCount, idx64) << Symbols::LF;
        const std::string sval = nextTemp(); out << std::format("  {} = load ptr, ptr {}", sval, elemPtr) << Symbols::LF;
        const std::string isptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x i8], ptr @gwb_data_isstr, i64 0, i64 {}", isptr, dataCount, idx64) << Symbols::LF;
        const std::string isb = nextTemp(); out << std::format("  {} = load i8, ptr {}", isb, isptr) << Symbols::LF;
        const std::string isStr = nextTemp(); out << std::format("  {} = icmp ne i8 {}, 0", isStr, isb) << Symbols::LF;
        const std::string idx1 = nextTemp(); out << std::format("  {} = add i32 {}, 1", idx1, idx) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_data_idx", idx1) << Symbols::LF;
        if (!target.indices.empty()) {
            const auto &dims = arrayDims_[target.name];
            long long total = 1;
            for (const int upperBound : dims) {
                long long extent = static_cast<long long>(upperBound) - optionBase_ + 1;
                extent = std::max(0LL, extent);
                total *= extent;
            }
            std::vector<std::string> idxI64s; idxI64s.reserve(target.indices.size());
            std::vector<std::string> bads; bads.reserve(target.indices.size());
            for (size_t di = 0; di < target.indices.size(); ++di) {
                const std::string idxD = emitExpr(out, target.indices[di].get(), "");
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
            const std::string doLbl2 = std::format("{}_read_arr_ok_{}", currLineLabel, ++localCounter);
            const std::string errLbl2 = std::format("{}_read_arr_err_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", anyBad, errLbl2, doLbl2) << Symbols::LF;
            out << errLbl2 << ":" << Symbols::LF;
            out << "  store i32 9, ptr @gwb_err_code" << Symbols::LF;
            out << std::format("  store i32 {}, ptr @gwb_err_line", currentLine_) << Symbols::LF;
            out << std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_) << Symbols::LF;
            out << std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex) << Symbols::LF;
            out << "  store i1 true, ptr @gwb_in_handler" << Symbols::LF;
            {
                const std::string trap = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF;
                out << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
                for (int lnum : lineNumbers_) { out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF; }
                out << "  ]" << Symbols::LF;
            }
            out << doLbl2 << ":" << Symbols::LF;
            std::vector<long long> extents; extents.reserve(dims.size());
            for (size_t di = 0; di < dims.size(); ++di) {
                long long extent = static_cast<long long>(dims[di]) - optionBase_ + 1;
                extent = std::max(0LL, extent);
                extents.push_back(extent);
            }
            std::vector<long long> strides(dims.size(), 1);
            for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) { strides[di] = strides[di + 1] * extents[di + 1]; }
            std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
            for (const auto &idxI64 : idxI64s) {
                const std::string adj = nextTemp();
                out << std::format("  {} = sub i64 {}, {}", adj, idxI64, optionBase_) << Symbols::LF;
                adjs.push_back(adj);
            }
            std::string lin = nextTemp(); out << std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]) << Symbols::LF;
            for (size_t di = 1; di < adjs.size(); ++di) {
                const std::string tmpProd = nextTemp();
                out << std::format("  {} = mul i64 {}, {}", tmpProd, adjs[di], strides[di]) << Symbols::LF;
                const std::string sumTmp = nextTemp();
                out << std::format("  {} = add i64 {}, {}", sumTmp, lin, tmpProd) << Symbols::LF;
                lin = sumTmp;
            }
            if (isStringArrayNameCG(target.name)) {
                ensureStringArrayAllocated(out, target.name, static_cast<int>(total));
                const std::string basea = arrayAllocaName_[target.name];
                const std::string elem = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, total, basea, lin) << Symbols::LF;
                out << std::format("  store ptr {}, ptr {}", sval, elem) << Symbols::LF;
            } else {
                ensureArrayAllocated(out, target.name, static_cast<int>(total));
                const std::string basea = arrayAllocaName_[target.name];
                const std::string elem = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, total, arrayElemType(target.name), basea, lin) << Symbols::LF;
                const std::string okNumLbl = std::format("{}_read_arr_ok_{}", currLineLabel, ++localCounter);
                const std::string errNumLbl = std::format("{}_read_arr_tyerr_{}", currLineLabel, localCounter);
                out << std::format("  br i1 {}, label %{}, label %{}", isStr, errNumLbl, okNumLbl) << Symbols::LF;
                out << errNumLbl << ":" << Symbols::LF;
                out << "  store i32 9, ptr @gwb_err_code" << Symbols::LF;
                out << std::format("  store i32 {}, ptr @gwb_err_line", currentLine_) << Symbols::LF;
                out << std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_) << Symbols::LF;
                out << std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex) << Symbols::LF;
                out << "  store i1 true, ptr @gwb_in_handler" << Symbols::LF;
                {
                    const std::string trap = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF;
                    out << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
                    for (int lnum : lineNumbers_) { out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF; }
                    out << "  ]" << Symbols::LF;
                }
                out << okNumLbl << ":" << Symbols::LF;
                const std::string nptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x double], ptr @gwb_data_num, i64 0, i64 {}", nptr, dataCount, idx64) << Symbols::LF;
                const std::string dval = nextTemp(); out << std::format("  {} = load double, ptr {}", dval, nptr) << Symbols::LF;
                storeNumberToVar(out, target.name, dval);
            }
        } else {
            // Scalar target
            if (isStringVarNameCG(target.name)) {
                ensureVarAllocated(out, target.name);
                out << std::format("  store ptr {}, ptr {}", sval, varAllocaName_[target.name]) << Symbols::LF;
            } else {
                const std::string nptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [{} x double], ptr @gwb_data_num, i64 0, i64 {}", nptr, dataCount, idx64) << Symbols::LF;
                const std::string dval = nextTemp(); out << std::format("  {} = load double, ptr {}", dval, nptr) << Symbols::LF;
                storeNumberToVar(out, target.name, dval);
            }
        }
    }
    (void)currLineLabel; (void)localCounter;
}

} // namespace gwbasic
