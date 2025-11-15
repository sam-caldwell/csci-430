// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/Program.h"
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: cdGatherLinesAndDeletes
 * Summary: Collect line numbers, map lines, deletes, and options.
 * Parameters:
 *  - program: Program AST to analyze.
 *  - linesOut: Output vector of kept line numbers (sorted, unique).
 *  - lineMapOut: Output mapping from line number to Line*.
 *  - printZones: Output flag set when OPTION PRINT ZONES is enabled.
 *  - deleteRanges: Output list of DELETE line ranges [start,end].
 *  - globalMin: Output minimum line number in program.
 *  - globalMax: Output maximum line number in program.
 * Returns:
 *  - void
 */
void CodeGenerator::cdGatherLinesAndDeletes(const Program& program, // NOLINT(readability-convert-member-functions-to-static,readability-function-size)
                                            std::vector<int>& linesOut,
                                            std::map<int, const Line*>& lineMapOut,
                                            bool& printZones,
                                            std::vector<std::pair<int,int>>& deleteRanges,
                                            LineBounds& bounds) {
    // Pass 1: collect lines, map, and min/max bounds
    cdCollectLinesAndBounds(program, linesOut, lineMapOut, bounds);
    std::ranges::sort(linesOut);
    linesOut.erase(std::ranges::unique(linesOut).begin(), linesOut.end());
    // Pass 2: collect delete ranges and options
    cdCollectDeleteAndOptions(program, printZones, deleteRanges, bounds);
    // Final normalization of bounds and ranges
    cdNormalizeBoundsAndRanges(bounds, deleteRanges);
}


} // namespace gwbasic
