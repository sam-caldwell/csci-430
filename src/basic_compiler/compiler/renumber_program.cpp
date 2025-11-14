// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/Program.h"

#include <algorithm>
#include <climits>

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
void renumberProgram(gwbasic::Program& program, const int base, int& outMinLine) {
    int minImported = INT_MAX;
    for (auto& [number, statements] : program.lines) {
        (void)statements;
        minImported = std::min(number, minImported);
        number += base;
    }
    outMinLine = (minImported == INT_MAX ? 0 : minImported);
}

} // namespace gwbasic::phase_log_helpers
