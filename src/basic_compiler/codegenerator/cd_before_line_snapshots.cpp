// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
// STL dependencies are provided via CodeGenerator.h

namespace gwbasic {

/*
 * Function: cdBuildBeforeLineSnapshots
 * Summary: Build snapshots of common/var/array sets before each line.
 * Parameters:
 *  - lines: Ordered line numbers to snapshot.
 * Returns:
 *  - void
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
