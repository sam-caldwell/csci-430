// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ReadStmt.h"

namespace gwbasic {

/* Collect variables for READ targets (scalars or arrays). */
void CodeGenerator::csvHandleRead(const ReadStmt* readStmt) { // NOLINT(readability-function-size)
    for (const auto& target : readStmt->targets) {
        if (!target.indices.empty()) {
            for (const auto& indexExpr : target.indices) {
                collectExprVars(indexExpr.get());
            }
        } else {
            variables_.insert(target.name);
        }
    }
}

} // namespace gwbasic
