// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/CommonStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdBuildBeforeLineSnapshots
 * Purpose: Build snapshots of COMMON variables and seen vars/arrays before each line.
 */
void CodeGenerator::cdBuildBeforeLineSnapshots(const std::vector<int>& lines) {
    std::set<std::string, std::less<>> accumCommon;
    std::set<std::string, std::less<>> accumVars;
    std::set<std::string, std::less<>> accumArrays;
    for (int ln : lines) {
        commonBeforeLine_[ln] = accumCommon;
        varsBeforeLine_[ln] = accumVars;
        arraysBeforeLine_[ln] = accumArrays;
        const auto* lptr = lineMap_[ln];
        if (!lptr) continue;
        for (const auto& st : lptr->statements) {
            if (const auto cs = dyn_cast<const CommonStmt>(st.get())) for (const auto& n : cs->names) accumCommon.insert(n);
            collectVarsForBeforeLineFromStmt(st.get(), accumVars, accumArrays);
        }
    }
}

} // namespace gwbasic

