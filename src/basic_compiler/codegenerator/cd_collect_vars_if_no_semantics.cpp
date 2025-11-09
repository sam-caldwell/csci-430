// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
// STL dependencies are provided via CodeGenerator.h

namespace gwbasic {

/*
 * Function: CodeGenerator::cdCollectVarsIfNoSemantics
 * Purpose: Walk kept lines and collect variables when semantics are not provided.
 */
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void CodeGenerator::cdCollectVarsIfNoSemantics(const std::vector<int>& lines) {
    if (semProvided_) {
        return;
    }
    for (const int lineNum : lines) {
        const auto* const linePtr = lineMap_[lineNum];
        if (linePtr == nullptr) {
            continue;
        }
        for (const auto& stmtNode : linePtr->statements) {
            collectStmtVars(stmtNode.get());
        }
    }
}

} // namespace gwbasic
