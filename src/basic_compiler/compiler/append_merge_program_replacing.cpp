// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"

namespace gwbasic::phase_log_helpers {

/*
 * Function: appendMergeProgramReplacing
 * Purpose:
 *  - Append lines from 'src' into 'dst', replacing duplicates by line number.
 * Inputs:
 *  - dst: Destination program
 *  - src: Source program (rvalue)
 */
void appendMergeProgramReplacing(gwbasic::Program& dst, gwbasic::Program&& src) {
    for (auto& ml : src.lines) replaceOrAppendLine(dst, std::move(ml), /*replace*/ true);
}

} // namespace gwbasic::phase_log_helpers

