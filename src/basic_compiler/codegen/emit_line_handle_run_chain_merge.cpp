// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/MergeStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/RunStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include <algorithm>
#include <format>
#include <functional>
#include <set>
#include <sstream>
#include <string>
#include <utility>

namespace gwbasic {

// RUN / CHAIN / MERGE. Returns true when terminates (RUN, CHAIN)
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity,readability-identifier-length)
bool CodeGenerator::emitLineHandleRunChainMerge(std::ostringstream &out, const Stmt *stmt) {
    if (const auto *runStmt = dyn_cast<RunStmt>(stmt)) {
        for (const auto &var : variables_) {
            if (varAllocaName_.contains(var)) { resetVar(out, var); }
        }
        const int fallback = lineNumbers_.empty() ? currentLine_ : lineNumbers_.front();
        const int dest = runStmt->targetLine.has_value() ? *runStmt->targetLine : fallback;
        const std::string branchIr = std::format("  br label %{}", lineLabelName(dest));
        out << branchIr << Symbols::LF;
        
        return true;
    }
    if (const auto *chainStmt = dyn_cast<ChainStmt>(stmt)) {
        if (!chainStmt->all) {
            const auto itCBL = commonBeforeLine_.find(currentLine_);
            const std::set<std::string, std::less<>> emptySet;
            const std::set<std::string, std::less<>> &preserve = (itCBL == commonBeforeLine_.end()) ? emptySet : itCBL->second;
            for (const auto &var : variables_) {
                if (preserve.contains(var)) { continue; }
                auto itVar = varAllocaName_.find(var);
                if (itVar == varAllocaName_.end()) { continue; }
                resetVar(out, var);
            }
            const auto itAB = arraysBeforeLine_.find(currentLine_);
            const std::set<std::string, std::less<>> emptyArr;
            const std::set<std::string, std::less<>> &aset = (itAB == arraysBeforeLine_.end()) ? emptyArr : itAB->second;
            for (const auto &arrayName : aset) {
                if (preserve.contains(arrayName)) { continue; }
                auto itLen = arrayDims_.find(arrayName);
                if (itLen == arrayDims_.end()) { continue; }
                long long len = 1;
                for (const int upperBound : itLen->second) {
                    long long ext = static_cast<long long>(upperBound) - optionBase_ + 1;
                    ext = std::max(0LL, ext);
                    len *= ext;
                }
                if (isStringArrayNameCG(arrayName)) {
                    ensureStringArrayAllocated(out, arrayName, static_cast<int>(len));
                    const std::string base = arrayAllocaName_[arrayName];
                    for (int i = 0; i < len; ++i) {
                        const std::string elem = nextTemp();
                        out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, len, base, i) << Symbols::LF;
                        out << std::format("  store ptr null, ptr {}", elem) << Symbols::LF;
                    }
                } else {
                    ensureArrayAllocated(out, arrayName, static_cast<int>(len));
                    const std::string base = arrayAllocaName_[arrayName];
                    for (int i = 0; i < len; ++i) {
                        const std::string elem = nextTemp();
                        out << std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, len, arrayElemType(arrayName), base, i) << Symbols::LF;
                        switch (numKindOf(arrayName)) {
                            case NumKind::Int16: { out << std::format("  store i32 0, ptr {}", elem) << Symbols::LF; break; }
                            case NumKind::Long32: { out << std::format("  store i64 0, ptr {}", elem) << Symbols::LF; break; }
                            case NumKind::Single: { out << std::format("  store float 0.0, ptr {}", elem) << Symbols::LF; break; }
                            case NumKind::Double: default: { out << std::format("  store double 0.0, ptr {}", elem) << Symbols::LF; break; }
                        }
                    }
                }
            }
        }
        const int fallback = lineNumbers_.empty() ? currentLine_ : lineNumbers_.front();
        const int dest = chainStmt->targetLine.has_value() ? *chainStmt->targetLine : fallback;
        // Reset DATA pointer to the beginning of the destination program segment
        const int region = (dest / 1000) * 1000;
        int dataStart = 0;
        auto itRegion = regionDataStartIdx_.find(region);
        if (itRegion != regionDataStartIdx_.end()) { dataStart = itRegion->second; }
        const std::string ir1 = std::format("  store i32 {}, ptr @gwb_data_idx", dataStart);
        out << ir1 << Symbols::LF;
        
        const std::string ir2 = std::format("  br label %{}", lineLabelName(dest));
        out << ir2 << Symbols::LF;
        
        return true;
    }
    if (isa<MergeStmt>(stmt)) {
        
        return false;
    }
    return false;
}

} // namespace gwbasic
