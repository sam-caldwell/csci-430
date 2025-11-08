// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/ResumeStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdComputeHandlerSkipAfter
 * Purpose: Determine the line to skip to after an ON ERROR handler when not in handler context.
 */
void CodeGenerator::cdComputeHandlerSkipAfter(const std::vector<int>& lines) {
    handlerSkipAfter_.clear();
    std::set<int> trapTargets;
    cdCollectTrapTargets(lines, trapTargets);
    for (int t : trapTargets) {
        const int startIdx = cdFindLineIndex(lines, t);
        if (startIdx < 0) continue;
        const int endIdx = cdFindResumeEndIdx(lines, startIdx);
        const int skipTo = cdComputeSkipFromIndices(lines, std::make_pair(startIdx, endIdx));
        handlerSkipAfter_[t] = skipTo;
    }
}

} // namespace gwbasic
