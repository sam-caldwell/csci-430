// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <vector>

namespace gwbasic {
    /*
     * Function: cdAssignInputPromptLiteralIds
     * Summary: Ensure INPUT prompt literals have stable ids.
     * Parameters:
     *  - lines: Candidate line numbers to scan for INPUT statements.
     * Returns:
     *  - void
     */
    // NOLINTNEXTLINE(readability-function-size,readability-convert-member-functions-to-static)
    void CodeGenerator::cdAssignInputPromptLiteralIds(const std::vector<int>& lines) {
        for (const int lineNum : lines) {
            const auto* const linePtr = lineMap_[lineNum];
            if (linePtr == nullptr) {
                continue;
            }

            for (const auto& stmtNode : linePtr->statements) {
                const auto* const inputStmt = dyn_cast<const InputStmt>(stmtNode.get());
                if (inputStmt == nullptr) {
                    continue;
                }
                if (!inputStmt->promptLiteral) {
                    continue;
                }
                if (strLiteralId_.contains(*inputStmt->promptLiteral)) {
                    continue;
                }

                strLiteralId_[*inputStmt->promptLiteral] = strCounter_++;
            }
        }
    }
} // namespace gwbasic
