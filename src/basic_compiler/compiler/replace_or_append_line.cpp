// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"

namespace gwbasic::phase_log_helpers {

/*
 * Function: replaceOrAppendLine
 * Purpose:
 *  - Replace a Program line with the same number or append when not found.
 * Inputs:
 *  - dst: Program to mutate
 *  - line: Line rvalue to store into the program
 *  - replace: Whether to replace on duplicate line numbers
 * Outputs:
 *  - void
 */
void replaceOrAppendLine(gwbasic::Program& dst, gwbasic::Line&& line, const bool replace) {
    if (replace) {
        for (auto& dl : dst.lines) {
            if (dl.number == line.number) { dl = std::move(line); return; }
        }
    }
    dst.lines.emplace_back(std::move(line));
}

} // namespace gwbasic::phase_log_helpers
