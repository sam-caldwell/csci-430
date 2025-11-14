// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/Program.h"

#include <utility>

namespace gwbasic::phase_log_helpers {

/*
 * Function: appendMergeProgramReplacing
 * Purpose:
 *  - Append lines from 'src' into 'dst', replacing duplicates by line number.
 * Inputs:
 *  - dst: Destination program
 *  - src: Source program (rvalue)
 */
// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
void appendMergeProgramReplacing(gwbasic::Program& dst, gwbasic::Program&& src) {
    auto lines = std::move(src.lines);
    for (auto& mergedLine : lines) {
        replaceOrAppendLine(dst, std::move(mergedLine), /*replace*/ true);
    }
}

} // namespace gwbasic::phase_log_helpers
