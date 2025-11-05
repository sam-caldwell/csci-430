// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/MergeStmt.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/RunStmt.h"
#include "basic_compiler/ast/RTTI.h"

namespace gwbasic::phase_log_helpers {

/*
 * Function: detectDirective
 * Purpose:
 *  - Inspect a line for MERGE/CHAIN/RUN directives and compute the include path.
 * Inputs:
 *  - ln: Line to inspect
 *  - curPath: Canonical path to the current source file
 *  - dir/outIncPath: Outputs for directive kind and resolved include path
 * Outputs:
 *  - bool: true if a directive was found; false otherwise
 */
bool detectDirective(const gwbasic::Line& ln,
                     const std::string& curPath,
                     Dir& dir,
                     std::string& outIncPath) {

    using enum gwbasic::phase_log_helpers::Dir;
    dir = None; outIncPath.clear();

    for (const auto& st : ln.statements) {
        if (const auto mg = gwbasic::dyn_cast<gwbasic::MergeStmt>(st.get())) {
            dir = Merge; outIncPath = resolvePath(curPath, mg->filename); return true;
        }
        if (const auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) {
            if (ch->filename.has_value()) { dir = Chain; outIncPath = resolvePath(curPath, *ch->filename); return true; }
        }
        if (const auto rn = gwbasic::dyn_cast<gwbasic::RunStmt>(st.get())) {
            if (rn->filename.has_value()) { dir = Run; outIncPath = resolvePath(curPath, *rn->filename); return true; }
        }
    }
    return false;

}

} // namespace gwbasic::phase_log_helpers
