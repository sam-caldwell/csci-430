// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DeleteStmt.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include <algorithm>
#include <limits>
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
// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void CodeGenerator::cdGatherLinesAndDeletes(const Program& program, // NOLINT(readability-convert-member-functions-to-static,readability-function-cognitive-complexity,readability-function-size)
                                            std::vector<int>& linesOut,
                                            std::map<int, const Line*>& lineMapOut,
                                            bool& printZones,
                                            std::vector<std::pair<int,int>>& deleteRanges,
                                            int& globalMin,
                                            int& globalMax) {
    globalMin = std::numeric_limits<int>::max();
    globalMax = std::numeric_limits<int>::min();
    linesOut.clear();
    lineMapOut.clear();
    for (const auto& line : program.lines) {
        linesOut.push_back(line.number);
        lineMapOut[line.number] = &line;
        globalMin = std::min(globalMin, line.number);
        globalMax = std::max(globalMax, line.number);
        for (const auto& stmtNode : line.statements) {
            if (const auto* const opz = dyn_cast<const OptionPrintZonesStmt>(stmtNode.get())) {
                printZones = opz->enabled;
            }
            if (const auto* const del = dyn_cast<const DeleteStmt>(stmtNode.get())) {
                int start = del->startLine.has_value() ? *del->startLine : globalMin;
                int end   = del->endLine.has_value()   ? *del->endLine   : globalMax;
                if (del->startIsDot) {
                    start = line.number;
                }
                if (del->endIsDot) {
                    end = line.number;
                }
                deleteRanges.emplace_back(start, end);
            }
        }
    }
    std::ranges::sort(linesOut);
    linesOut.erase(std::ranges::unique(linesOut).begin(), linesOut.end());
    if (globalMin == std::numeric_limits<int>::max()) {
        globalMin = 0;
    }
    if (globalMax == std::numeric_limits<int>::min()) {
        globalMax = 0;
    }
    // Normalize open-ended ranges based on discovered global bounds
    for (auto& range : deleteRanges) {
        if (range.first < globalMin) {
            range.first = globalMin;
        }
        if (range.second < range.first) {
            range.second = range.first;
        }
        if (range.second > globalMax) {
            range.second = globalMax;
        }
    }
}

// NOLINTEND(bugprone-easily-swappable-parameters)

} // namespace gwbasic
