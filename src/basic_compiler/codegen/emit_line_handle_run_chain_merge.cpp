// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/RunStmt.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/MergeStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>
#include <string>
#include <utility>

namespace gwbasic {

// RUN / CHAIN / MERGE. Returns true when terminates (RUN, CHAIN)
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity,readability-identifier-length)
bool CodeGenerator::emitLineHandleRunChainMerge(std::ostringstream &out, const Stmt *stmt) {
    if (const auto *rn = dyn_cast<RunStmt>(stmt)) {
        for (const auto &v : variables_) {
            if (varAllocaName_.contains(v)) { resetVar(out, v); }
        }
        const int dest = rn->targetLine.has_value()
                             ? *rn->targetLine
                             : (lineNumbers_.empty() ? currentLine_ : lineNumbers_.front());
        const std::string ir = std::format("  br label %{}", lineLabelName(dest));
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " RunStmt branch -> " << ir << Symbols::LF;
        return true;
    }
    if (const auto *ch = dyn_cast<ChainStmt>(stmt)) {
        if (!ch->all) {
            const auto itCBL = commonBeforeLine_.find(currentLine_);
            const std::set<std::string, std::less<>> emptySet;
            const std::set<std::string, std::less<>> &preserve = (itCBL == commonBeforeLine_.end()) ? emptySet : itCBL->second;
            for (const auto &v : variables_) {
                if (preserve.contains(v)) { continue; }
                auto it = varAllocaName_.find(v);
                if (it == varAllocaName_.end()) { continue; }
                resetVar(out, v);
            }
            const auto itAB = arraysBeforeLine_.find(currentLine_);
            const std::set<std::string, std::less<>> emptyArr;
            const std::set<std::string, std::less<>> &aset = (itAB == arraysBeforeLine_.end()) ? emptyArr : itAB->second;
            for (const auto &an : aset) {
                if (preserve.contains(an)) { continue; }
                auto itLen = arrayDims_.find(an);
                if (itLen == arrayDims_.end()) { continue; }
                long long len = 1;
                for (int ub : itLen->second) {
                    long long ext = (static_cast<long long>(ub) - optionBase_ + 1);
                    if (ext < 0) { ext = 0; }
                    len *= ext;
                }
                if (isStringArrayNameCG(an)) {
                    ensureStringArrayAllocated(out, an, static_cast<int>(len));
                    const std::string base = arrayAllocaName_[an];
                    for (int i = 0; i < len; ++i) {
                        const std::string elem = nextTemp();
                        out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, len, base, i) << Symbols::LF;
                        out << std::format("  store ptr null, ptr {}", elem) << Symbols::LF;
                    }
                } else {
                    ensureArrayAllocated(out, an, static_cast<int>(len));
                    const std::string base = arrayAllocaName_[an];
                    for (int i = 0; i < len; ++i) {
                        const std::string elem = nextTemp();
                        out << std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, len, arrayElemType(an), base, i) << Symbols::LF;
                        switch (numKindOf(an)) {
                            case NumKind::Int16: { out << std::format("  store i32 0, ptr {}", elem) << Symbols::LF; break; }
                            case NumKind::Long32: { out << std::format("  store i64 0, ptr {}", elem) << Symbols::LF; break; }
                            case NumKind::Single: { out << std::format("  store float 0.0, ptr {}", elem) << Symbols::LF; break; }
                            case NumKind::Double: default: { out << std::format("  store double 0.0, ptr {}", elem) << Symbols::LF; break; }
                        }
                    }
                }
            }
        }
        const int dest = ch->targetLine.has_value()
                             ? *ch->targetLine
                             : (lineNumbers_.empty() ? currentLine_ : lineNumbers_.front());
        // Reset DATA pointer to the beginning of the destination program segment
        const int region = (dest / 1000) * 1000;
        int dataStart = 0;
        auto it = regionDataStartIdx_.find(region);
        if (it != regionDataStartIdx_.end()) { dataStart = it->second; }
        const std::string ir1 = std::format("  store i32 {}, ptr @gwb_data_idx", dataStart);
        out << ir1 << Symbols::LF;
        log() << "line " << currentLine_ << " ChainStmt data_idx -> " << ir1 << Symbols::LF;
        const std::string ir2 = std::format("  br label %{}", lineLabelName(dest));
        out << ir2 << Symbols::LF;
        log() << "line " << currentLine_ << " ChainStmt branch -> " << ir2 << Symbols::LF;
        return true;
    }
    if (isa<MergeStmt>(stmt)) {
        log() << "line " << currentLine_ << " MergeStmt (no-op)" << Symbols::LF;
        return false;
    }
    return false;
}

} // namespace gwbasic
