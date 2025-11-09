// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdComputeSkipFromIndices
 * Purpose: Compute the skip-to line number based on handler start/end indices.
 */
int CodeGenerator::cdComputeSkipFromIndices(const std::vector<int>& lines, const std::pair<int,int> &idx) {
    const int startIdx = idx.first;
    if (const int endIdx = idx.second; endIdx >= 0) {
        if (const int next = endIdx + 1; next < static_cast<int>(lines.size())) {
            return lines[next];
        }
        return -1;
    }
    if (const int next = startIdx + 1; next < static_cast<int>(lines.size())) {
        return lines[next];
    }
    return -1;
}

} // namespace gwbasic
