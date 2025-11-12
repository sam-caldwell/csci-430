// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <set>

namespace gwbasic {
    /*
     * Function: cdMaybeAddTrapTarget
     * Summary: Insert ON ERROR GOTO target into the set if present.
     * Parameters:
     *  - stmt: Statement to inspect.
     *  - trapTargets: Set to receive the target line number when > 0.
     * Returns:
     *  - void
     */
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void CodeGenerator::cdMaybeAddTrapTarget(const Stmt* stmt, std::set<int>& trapTargets) {
        const auto* oeg = dyn_cast<const OnErrorGotoStmt>(stmt); // level 1
        if (oeg == nullptr) { return; }

        if (const int tgt = oeg->targetLine; tgt > 0) { // level 2
            trapTargets.insert(tgt);
        }
    }

} // namespace gwbasic
