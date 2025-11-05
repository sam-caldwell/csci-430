// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"

namespace gwbasic::phase_log_helpers {

/*
 * Function: renumberProgram
 * Purpose:
 *  - Add a base offset to each line number and compute the minimum prior number.
 * Inputs:
 *  - prog: Program to mutate
 *  - base: Base offset to add
 * Outputs:
 *  - outMinLine: Minimum original line number (0 when empty)
 */
void renumberProgram(gwbasic::Program& prog, const int base, int& outMinLine) {
    int minImported = INT_MAX;
    for (auto& [number, statements] : prog.lines) {
        (void)statements;
        if (number < minImported) minImported = number;
        number += base;
    }
    outMinLine = (minImported == INT_MAX ? 0 : minImported);
}

} // namespace gwbasic::phase_log_helpers

