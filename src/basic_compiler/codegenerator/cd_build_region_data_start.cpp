// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: cdBuildRegionDataStartIdx
 * Summary: Map each 1000-based region to the DATA item start index.
 * Parameters:
 *  - lines: Ordered kept line numbers to scan.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-function-size,readability-convert-member-functions-to-static)
void CodeGenerator::cdBuildRegionDataStartIdx(const std::vector<int>& lines) {
    regionDataStartIdx_.clear();
    int dataCount = 0; // NOLINT(misc-const-correctness)
    for (const int lineNum : lines) {
        if (const int region = (lineNum / 1000) * 1000; !regionDataStartIdx_.contains(region)) {
            regionDataStartIdx_[region] = dataCount;
        }
        const auto* const linePtr = lineMap_[lineNum];
        if (linePtr == nullptr) {
            continue;
        }
        for (const auto& stmtNode : linePtr->statements) {
            if (const auto* const dataStmt = dyn_cast<const DataStmt>(stmtNode.get())) {
                dataCount += static_cast<int>(dataStmt->items.size());
            }
        }
    }
}

} // namespace gwbasic
