// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/Program.h"
#include <algorithm>
#include <utility>

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
        const auto foundIt = std::find_if(dst.lines.begin(), dst.lines.end(),
                                          [&](const gwbasic::Line& dstLine) { return dstLine.number == line.number; });
        if (foundIt != dst.lines.end()) {
            *foundIt = std::move(line);
            return;
        }
    }
    dst.lines.emplace_back(std::move(line));
}

} // namespace gwbasic::phase_log_helpers
