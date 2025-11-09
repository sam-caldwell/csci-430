// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/RTTI.h"

namespace gwbasic {

/*
 * Function: cdCollectDataItemsFromLine
 * Summary: Collect DATA items from a single line by number.
 * Parameters:
 *  - lineNum: Source line number to inspect.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void CodeGenerator::cdCollectDataItemsFromLine(const int lineNum) {
    const auto* const linePtr = lineMap_[lineNum];
    if (linePtr == nullptr) {
        return;
    }
    for (const auto& stmtNode : linePtr->statements) {
        if (const auto* const dataStmt = dyn_cast<const DataStmt>(stmtNode.get())) {
            cdCollectDataItemsFromStmt(*dataStmt);
        }
    }
}

} // namespace gwbasic
