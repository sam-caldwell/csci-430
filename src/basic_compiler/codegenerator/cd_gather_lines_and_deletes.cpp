// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
#include "basic_compiler/ast/DeleteStmt.h"
#include <algorithm>
#include <limits>

namespace gwbasic {

/*
 * Function: CodeGenerator::cdGatherLinesAndDeletes
 * Purpose:
 *  - Populate `linesOut` and `lineMapOut` from the program and collect DELETE ranges.
 *  - Also detects OPTION PRINT ZONES.
 */
void CodeGenerator::cdGatherLinesAndDeletes(const Program& program,
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
        for (const auto& st : line.statements) {
            if (const auto* opz = dyn_cast<const OptionPrintZonesStmt>(st.get())) printZones = opz->enabled;
            if (const auto* del = dyn_cast<const DeleteStmt>(st.get())) {
                int start = del->startLine.has_value() ? *del->startLine : globalMin;
                int end   = del->endLine.has_value()   ? *del->endLine   : globalMax;
                if (del->startIsDot) start = line.number;
                if (del->endIsDot)   end   = line.number;
                deleteRanges.emplace_back(start, end);
            }
        }
    }
    std::ranges::sort(linesOut);
    linesOut.erase(std::ranges::unique(linesOut).begin(), linesOut.end());
    if (globalMin == std::numeric_limits<int>::max()) globalMin = 0;
    if (globalMax == std::numeric_limits<int>::min()) globalMax = 0;
    // Normalize open-ended ranges based on discovered global bounds
    for (auto& r : deleteRanges) {
        if (r.first < globalMin) r.first = globalMin;
        if (r.second < r.first)  r.second = r.first;
        if (r.second > globalMax) r.second = globalMax;
    }
}

} // namespace gwbasic
