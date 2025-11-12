// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <set>
#include <vector>

namespace gwbasic {

    /*
     * Function: cdCollectTrapTargets
     * Summary: Collect positive ON ERROR GOTO targets from kept lines.
     * Parameters:
     *  - lines: Ordered line numbers to scan.
     *  - trapTargets: Output set populated with unique positive target lines.
     * Returns:
     *  - void
     */
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void CodeGenerator::cdCollectTrapTargets(const std::vector<int>& lines, std::set<int>& trapTargets) {

        for (const int lineNum : lines) { // level 1

            const auto foundIt = lineMap_.find(lineNum);

            if (foundIt == lineMap_.end() || foundIt->second == nullptr) {
                continue;
            }

            for (const auto* linePtr = foundIt->second; const auto& stmtNode : linePtr->statements) { // level 2
                cdMaybeAddTrapTarget(stmtNode.get(), trapTargets);
            }
        }
    }

} // namespace gwbasic
