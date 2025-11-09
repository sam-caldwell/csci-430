// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"

namespace gwbasic {

    /**
     * @brief Collect positive OnErrorGoto targets from the given lines.
     *
     * Keeps nesting under three levels and avoids mutating lineMap_ on misses.
     *
     * @param lines       Ordered line numbers to scan.
     * @param trapTargets Output set populated with unique positive target lines.
     */
    void CodeGenerator::cdCollectTrapTargets(const std::vector<int>& lines, std::set<int>& trapTargets) {
        for (int ln : lines) { // level 1
            const auto it = lineMap_.find(ln);
            if (it == lineMap_.end() || it->second == nullptr) continue;

            const auto* lptr = it->second;
            for (const auto& st : lptr->statements) { // level 2
                cdMaybeAddTrapTarget(st.get(), trapTargets);
            }
        }
    }

} // namespace gwbasic

