// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/InputStmt.h"

namespace gwbasic {
    /*
     * Function: CodeGenerator::cdAssignInputPromptLiteralIds
     * Purpose: Ensure INPUT prompt literals have stable ids.
     */
    void CodeGenerator::cdAssignInputPromptLiteralIds(const std::vector<int> &lines) {
        for (int ln: lines) {
            const auto *lptr = lineMap_[ln];
            if (!lptr) continue;

            for (const auto &st: lptr->statements) {
                const auto *in = dyn_cast<const InputStmt>(st.get());
                if (!in) continue;
                if (!in->promptLiteral) continue;
                if (strLiteralId_.contains(*in->promptLiteral)) continue;

                strLiteralId_[*in->promptLiteral] = strCounter_++;
            }
        }
    }
} // namespace gwbasic
