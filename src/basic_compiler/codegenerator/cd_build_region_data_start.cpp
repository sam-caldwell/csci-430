// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DataStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdBuildRegionDataStartIdx
 * Purpose: Map each 1000-based region to DATA item index at its first line.
 */
void CodeGenerator::cdBuildRegionDataStartIdx(const std::vector<int>& lines) {
    regionDataStartIdx_.clear();
    int dataCount = 0;
    for (int ln : lines) {
        if (const int region = (ln / 1000) * 1000; !regionDataStartIdx_.contains(region))
            regionDataStartIdx_[region] = dataCount;
        const auto* lptr = lineMap_[ln]; if (!lptr) continue;
        for (const auto& st : lptr->statements) {
            if (const auto ds = dyn_cast<const DataStmt>(st.get()))
                dataCount += static_cast<int>(ds->items.size());
        }
    }
}

} // namespace gwbasic
