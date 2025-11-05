// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/RunStmt.h"
#include "basic_compiler/ast/RTTI.h"

namespace gwbasic::phase_log_helpers {

/*
 * Function: patchTargetsForChainOrRun
 * Purpose:
 *  - Patch the CHAIN or RUN statement in a line to jump to base + target.
 * Inputs:
 *  - ln: Line to modify
 *  - importInfo: {base, minLine} for the imported program
 *  - isChain: Whether to patch CHAIN (true) or RUN (false)
 */
void patchTargetsForChainOrRun(const gwbasic::Line& ln,
                               const std::pair<int,int>& importInfo,
                               const bool isChain) {
    const int base = importInfo.first;
    const int first = importInfo.second;
    if (isChain) {
        for (auto const& st : ln.statements) {
            if (const auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) {
                const int tgt = ch->targetLine.has_value() ? *ch->targetLine : first;
                ch->targetLine = base + tgt;
                break;
            }
        }
    } else {
        for (auto const& st : ln.statements) {
            if (const auto rn = gwbasic::dyn_cast<gwbasic::RunStmt>(st.get())) {
                const int tgt = rn->targetLine.has_value() ? *rn->targetLine : first;
                rn->targetLine = base + tgt;
                break;
            }
        }
    }
}

} // namespace gwbasic::phase_log_helpers

