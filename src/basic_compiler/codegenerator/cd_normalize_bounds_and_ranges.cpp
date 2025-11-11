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
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void CodeGenerator::cdNormalizeBoundsAndRanges(int& globalMin,
                                               int& globalMax,
                                               std::vector<std::pair<int,int>>& deleteRanges) {
    if (globalMin == std::numeric_limits<int>::max()) { globalMin = 0; }
    if (globalMax == std::numeric_limits<int>::min()) { globalMax = 0; }
    for (auto& rng : deleteRanges) {
        int& fst = rng.first;
        int& snd = rng.second;
        fst = std::max(fst, globalMin);
        snd = std::max(snd, fst);
        snd = std::min(snd, globalMax);
    }
}

} // namespace gwbasic
