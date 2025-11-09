// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: CodeGenerator::cdComputeSkipFromIndices
 * Purpose: Compute the skip-to line number based on handler start/end indices.
 */
int CodeGenerator::cdComputeSkipFromIndices(const std::vector<int>& lines, std::pair<int,int> idx) {
    const int startIdx = idx.first;
    const int endIdx = idx.second;
    if (endIdx >= 0) {
        const int next = endIdx + 1;
        if (next < static_cast<int>(lines.size())) {
            return lines[next];
        }
        return -1;
    }
    const int next = startIdx + 1;
    if (next < static_cast<int>(lines.size())) {
        return lines[next];
    }
    return -1;
}

} // namespace gwbasic
