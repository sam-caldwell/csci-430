// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
// STL dependencies are provided via CodeGenerator.h

namespace gwbasic {

/*
 * Function: CodeGenerator::cdComputeHandlerSkipAfter
 * Purpose: Determine the line to skip to after an ON ERROR handler when not in handler context.
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
