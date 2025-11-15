// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: cdNormalizeBoundsAndRanges
 * Summary: Normalize global min/max and clamp ranges to [globalMin, globalMax].
 * Parameters:
 *  - globalMin/globalMax: In/out bounds.
 *  - deleteRanges: In/out ranges clamped to normalized bounds.
 * Returns:
 *  - void
 */
void CodeGenerator::cdNormalizeBoundsAndRanges(LineBounds& bounds,
                                               std::vector<std::pair<int,int>>& deleteRanges) {
    if (bounds.min == std::numeric_limits<int>::max()) { bounds.min = 0; }
    if (bounds.max == std::numeric_limits<int>::min()) { bounds.max = 0; }
    for (auto&[fst, snd] : deleteRanges) {
        fst = std::max(fst, bounds.min);
        snd = std::max(snd, fst);
        snd = std::min(snd, bounds.max);
    }
}

} // namespace gwbasic
