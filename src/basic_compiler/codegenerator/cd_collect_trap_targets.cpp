// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdCollectTrapTargets
 * Purpose: Collect unique ON ERROR GOTO target line numbers.
 */
void CodeGenerator::cdCollectTrapTargets(const std::vector<int>& lines, std::set<int>& trapTargets) {
    for (int ln : lines) {
        const auto* lptr = lineMap_[ln];
        if (!lptr) continue;
        for (const auto& st : lptr->statements) {
            if (const auto oeg = dyn_cast<const OnErrorGotoStmt>(st.get())) {
                if (oeg->targetLine > 0) trapTargets.insert(oeg->targetLine);
            }
        }
    }
}

} // namespace gwbasic

