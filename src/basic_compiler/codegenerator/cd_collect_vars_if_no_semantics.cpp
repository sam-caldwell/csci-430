// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdCollectVarsIfNoSemantics
 * Purpose: Walk kept lines and collect variables when semantics are not provided.
 */
void CodeGenerator::cdCollectVarsIfNoSemantics(const std::vector<int>& lines) {
    if (semProvided_) return;
    for (int ln : lines) {
        const auto* lptr = lineMap_[ln];
        if (!lptr) continue;
        for (const auto& st : lptr->statements) collectStmtVars(st.get());
    }
}

} // namespace gwbasic

