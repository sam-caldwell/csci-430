// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"

namespace gwbasic {
    /**
     * @brief Insert the OnErrorGoto target into trapTargets if present and valid.
     *
     * @param st          Statement to inspect.
     * @param trapTargets Set that receives the target line if > 0.
     */
    void CodeGenerator::cdMaybeAddTrapTarget(const Stmt* st, std::set<int>& trapTargets) {
        const auto* oeg = dyn_cast<const OnErrorGotoStmt>(st); // level 1
        if (!oeg) return;

        const int tgt = oeg->targetLine;
        if (tgt > 0) { // level 2
            trapTargets.insert(tgt);
        }
    }

} // namespace gwbasic

