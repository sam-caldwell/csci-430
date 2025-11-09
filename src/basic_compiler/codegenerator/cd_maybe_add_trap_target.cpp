// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {
    /**
     * @brief Insert the OnErrorGoto target into trapTargets if present and valid.
     *
     * @param st          Statement to inspect.
     * @param trapTargets Set that receives the target line if > 0.
     */
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void CodeGenerator::cdMaybeAddTrapTarget(const Stmt* stmt, std::set<int>& trapTargets) {
        const auto* oeg = dyn_cast<const OnErrorGotoStmt>(stmt); // level 1
        if (!oeg) { return; }

        if (const int tgt = oeg->targetLine; tgt > 0) { // level 2
            trapTargets.insert(tgt);
        }
    }

} // namespace gwbasic
