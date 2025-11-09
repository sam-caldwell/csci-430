// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
// STL dependencies are provided via CodeGenerator.h

namespace gwbasic {

/*
 * Function: cdFilterDeletedLines
 * Summary: Remove line numbers that fall within any DELETE range.
 * Parameters:
 *  - lines: In/out vector of kept line numbers.
 *  - deleteRanges: List of [start,end] ranges to remove.
 * Returns:
 *  - void
 */
// NOLINT(readability-convert-member-functions-to-static,readability-function-size)
void CodeGenerator::cdFilterDeletedLines(std::vector<int>& lines, const std::vector<std::pair<int,int>>& deleteRanges) {
    if (deleteRanges.empty()) {
        return;
    }
    std::set<int> deleted;
    for (const int lineNum : lines) {
        for (const auto&[fst, snd] : deleteRanges) {
            if (lineNum >= fst && lineNum <= snd) {
                deleted.insert(lineNum);
                break;
            }
        }
    }
    if (deleted.empty()) {
        return;
    }
    std::vector<int> kept;
    kept.reserve(lines.size());
    for (const int lineNum : lines) {
        if (!deleted.contains(lineNum)) {
            kept.push_back(lineNum);
        }
    }
    lines.swap(kept);
}

} // namespace gwbasic
