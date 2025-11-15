// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/CommonStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

namespace gwbasic {

    /*
     * Function: cdAccumulateFromStatement
     * Summary: Update before-line accumulators from a single statement.
     * Parameters:
     *  - stmt: Statement to analyze.
     *  - accumCommon: Accumulator set for COMMON names.
     *  - accumVars: Accumulator set for variable identifiers.
     *  - accumArrays: Accumulator set for array identifiers.
     * Returns:
     *  - void
     */
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void CodeGenerator::cdAccumulateFromStatement(
        const Stmt* stmt,
        AccumRefs acc){

        if (const auto* const commonStmt = dyn_cast<const CommonStmt>(stmt)) { // level 1
            for (const auto& name : commonStmt->names) {                        // level 2
                acc.common.insert(name);
            }
        }
        // Delegated extraction; may update both vars and arrays.
        collectVarsForBeforeLineFromStmt(stmt, acc.vars, acc.arrays);
    }
} // namespace gwbasic
