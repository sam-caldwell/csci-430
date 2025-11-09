// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

    /**
     * @brief Update accumulators from a single statement.
     *
     * Adds names from CommonStmt to @p accumCommon and forwards variable/array
     * discovery to collectVarsForBeforeLineFromStmt().
     *
     * @param stmt         Pointer to the statement to analyze.
     * @param accumCommon  Accumulator for common names discovered so far.
     * @param accumVars    Accumulator for variable identifiers discovered so far.
     * @param accumArrays  Accumulator for array identifiers discovered so far.
     */
    // NOLINTBEGIN(bugprone-easily-swappable-parameters)
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void CodeGenerator::cdAccumulateFromStatement(
        const Stmt* stmt,
        std::set<std::string, std::less<>>& accumCommon,
        std::set<std::string, std::less<>>& accumVars,
        std::set<std::string, std::less<>>& accumArrays){

        if (const auto* const commonStmt = dyn_cast<const CommonStmt>(stmt)) { // level 1
            for (const auto& name : commonStmt->names) {                        // level 2
                accumCommon.insert(name);
            }
        }
        // Delegated extraction; may update both vars and arrays.
        collectVarsForBeforeLineFromStmt(stmt, accumVars, accumArrays);
    }
    // NOLINTEND(bugprone-easily-swappable-parameters)
} // namespace gwbasic
