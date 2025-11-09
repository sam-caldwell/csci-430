// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
// STL dependencies are provided via CodeGenerator.h

namespace gwbasic {

/**
 * @brief Accumulate snapshot data for all provided line numbers.
 *
 * Maintains the same behavior as the original implementation while keeping
 * nesting to at most three levels. For each line, this records the current
 * accumulated sets of commons, vars, and arrays before visiting that line's
 * statements, then updates the accumulators from the statements (if any).
 *
 * @param lines Ordered collection of line numbers to snapshot before.
 */
void CodeGenerator::cdBuildBeforeLineSnapshots(const std::vector<int>& lines) {
    for (const int lineNum : lines) {
        std::set<std::string, std::less<>> accumCommon;
        std::set<std::string, std::less<>> accumVars;
        std::set<std::string, std::less<>> accumArrays;
        // level 1
        // Record snapshots as of "before this line".
        commonBeforeLine_[lineNum] = accumCommon;
        varsBeforeLine_[lineNum]   = accumVars;
        arraysBeforeLine_[lineNum] = accumArrays;

        const auto* linePtr = lineMap_[lineNum];
        if (linePtr == nullptr) {
            continue;
        }

        for (const auto& stmtNode : linePtr->statements) { // level 2
            CodeGenerator::cdAccumulateFromStatement(stmtNode.get(), accumCommon, accumVars, accumArrays);
        }
    }
}

} // namespace gwbasic
