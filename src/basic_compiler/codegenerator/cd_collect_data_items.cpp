// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DataStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdCollectDataItems
 * Purpose: Flatten DATA items into tables and assign literal ids.
 */
void CodeGenerator::cdCollectDataItems(const std::vector<int>& lines) {
    dataLiteralIds_.clear();
    dataIsString_.clear();
    dataNumValues_.clear();
    for (int ln : lines) { cdCollectDataItemsFromLine(ln); }
}

} // namespace gwbasic
