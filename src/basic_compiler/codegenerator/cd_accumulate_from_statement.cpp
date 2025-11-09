// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/CommonStmt.h"

namespace gwbasic {

    /**
     * @brief Update accumulators from a single statement.
     *
     * Adds names from CommonStmt to @p accumCommon and forwards variable/array
     * discovery to collectVarsForBeforeLineFromStmt().
     *
     * @param st           Pointer to the statement to analyze.
     * @param accumCommon  Accumulator for common names discovered so far.
     * @param accumVars    Accumulator for variable identifiers discovered so far.
     * @param accumArrays  Accumulator for array identifiers discovered so far.
     */
    void CodeGenerator::cdAccumulateFromStatement(
        const Stmt* st,
        std::set<std::string, std::less<>>& accumCommon,
        std::set<std::string, std::less<>>& accumVars,
        std::set<std::string, std::less<>>& accumArrays){

        if (const auto cs = dyn_cast<const CommonStmt>(st)) { // level 1
            for (const auto& n : cs->names) {                 // level 2
                accumCommon.insert(n);
            }
        }
        // Delegated extraction; may update both vars and arrays.
        collectVarsForBeforeLineFromStmt(st, accumVars, accumArrays);
    }
} // namespace gwbasic
