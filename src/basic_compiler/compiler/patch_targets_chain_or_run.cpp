// (c) 2025 Sam Caldwell. All Rights Reserved.
// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
// NOLINTBEGIN(llvm-include-order)
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/RunStmt.h"
// NOLINTEND(llvm-include-order)
#include "basic_compiler/ast/Line.h"

#include <utility>

namespace gwbasic::phase_log_helpers {

/*
 * Function: patchTargetsForChainOrRun
 * Purpose:
 *  - Patch the CHAIN or RUN statement in a line to jump to base + target.
 * Inputs:
 *  - lineObj: Line to modify
 *  - importInfo: {base, minLine} for the imported program
 *  - isChain: Whether to patch CHAIN (true) or RUN (false)
 */
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity,misc-include-cleaner)
void patchTargetsForChainOrRun(const gwbasic::Line& lineObj,
                                const std::pair<int,int>& importInfo,
                                const bool isChain) {
    const int base = importInfo.first;
    const int first = importInfo.second;
    if (isChain) {
        for (auto const& stmtPtr : lineObj.statements) {
            if (auto* const chainStmt = gwbasic::dyn_cast<gwbasic::ChainStmt>(stmtPtr.get())) {
                const int target = chainStmt->targetLine.has_value() ? *chainStmt->targetLine : first;
                chainStmt->targetLine = base + target;
                break;
            }
        }
    } else {
        for (auto const& stmtPtr : lineObj.statements) {
            if (auto* const runStmt = gwbasic::dyn_cast<gwbasic::RunStmt>(stmtPtr.get())) {
                const int target = runStmt->targetLine.has_value() ? *runStmt->targetLine : first;
                runStmt->targetLine = base + target;
                break;
            }
        }
    }
}

} // namespace gwbasic::phase_log_helpers
