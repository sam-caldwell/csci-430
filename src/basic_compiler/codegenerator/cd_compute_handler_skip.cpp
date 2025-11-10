// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <set>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: cdComputeHandlerSkipAfter
 * Summary: Compute the post-handler skip line for ON ERROR handlers.
 * Parameters:
 *  - lines: Kept line numbers to scan.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void CodeGenerator::cdComputeHandlerSkipAfter(const std::vector<int>& lines) {
    handlerSkipAfter_.clear();
    std::set<int> trapTargets;
    cdCollectTrapTargets(lines, trapTargets);
    for (const int lineNum : trapTargets) {
        const int startIdx = cdFindLineIndex(lines, lineNum);
        if (startIdx < 0) {
            continue;
        }
        const int endIdx = cdFindResumeEndIdx(lines, startIdx);
        const int skipTo = cdComputeSkipFromIndices(lines, std::make_pair(startIdx, endIdx));
        handlerSkipAfter_[lineNum] = skipTo;
    }
}

} // namespace gwbasic
