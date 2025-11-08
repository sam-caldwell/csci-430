// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DataStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdCollectDataItemsFromLine
 * Purpose: Collect DATA items from a single line.
 */
void CodeGenerator::cdCollectDataItemsFromLine(int ln) {
    const auto* lptr = lineMap_[ln];
    if (!lptr) return;
    for (const auto& st : lptr->statements) {
        if (const auto ds = dyn_cast<const DataStmt>(st.get())) {
            cdCollectDataItemsFromStmt(*ds);
        }
    }
}

} // namespace gwbasic

