// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/Program.h"
#include <limits>
#include <map>
#include <vector>

namespace gwbasic {

/*
 * Function: cdCollectLinesAndBounds
 * Summary: Collect line numbers, line map, and global min/max.
 * Parameters:
 *  - program: Program AST to scan.
 *  - linesOut: Output vector of line numbers (not unique or sorted).
 *  - lineMapOut: Output mapping from line number to Line*.
 *  - globalMin/globalMax: Updated bounds across all lines.
 * Returns:
 *  - void
 */
// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void CodeGenerator::cdCollectLinesAndBounds(const Program& program,
                                            std::vector<int>& linesOut,
                                            std::map<int, const Line*>& lineMapOut,
                                            int& globalMin,
                                            int& globalMax) {
    globalMin = std::numeric_limits<int>::max();
    globalMax = std::numeric_limits<int>::min();
    linesOut.clear();
    lineMapOut.clear();
    for (const auto& line : program.lines) {
        linesOut.push_back(line.number);
        lineMapOut[line.number] = &line;
        if (line.number < globalMin) {
            globalMin = line.number;
        }
        if (line.number > globalMax) {
            globalMax = line.number;
        }
    }
}
// NOLINTEND(bugprone-easily-swappable-parameters)

} // namespace gwbasic
