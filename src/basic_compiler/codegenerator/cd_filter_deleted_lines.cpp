// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdFilterDeletedLines
 * Purpose: Remove lines that fall within any DELETE range.
 */
void CodeGenerator::cdFilterDeletedLines(std::vector<int>& lines,
                                         const std::vector<std::pair<int,int>>& deleteRanges) {
    if (deleteRanges.empty()) return;
    std::set<int> deleted;
    for (int ln : lines) {
        for (const auto& r : deleteRanges) {
            if (ln >= r.first && ln <= r.second) { deleted.insert(ln); break; }
        }
    }
    if (deleted.empty()) return;
    std::vector<int> kept; kept.reserve(lines.size());
    for (int ln : lines) if (!deleted.contains(ln)) kept.push_back(ln);
    lines.swap(kept);
}

} // namespace gwbasic

