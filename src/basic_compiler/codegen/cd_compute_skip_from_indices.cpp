// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <utility>
#include <vector>

namespace gwbasic {
    /*
     * Function: cdComputeSkipFromIndices
     * Summary: Compute the skip-to line number from handler start/end indices.
     * Parameters:
     *  - lines: Ordered kept line numbers.
     *  - idx: Pair of (startIdx, endIdx) into lines; endIdx may be -1.
     * Returns:
     *  - int: Next line number to resume at, or -1 if none.
     */
    int CodeGenerator::cdComputeSkipFromIndices(const std::vector<int> &lines, const std::pair<int, int> &index_pair) {
        const int startIdx = index_pair.first;

        if (const int endIdx = index_pair.second; endIdx >= 0) {
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
