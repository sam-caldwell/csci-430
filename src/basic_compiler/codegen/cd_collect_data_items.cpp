// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <vector>
// All needed types are available via CodeGenerator.h

namespace gwbasic {

/*
 * Function: cdCollectDataItems
 * Summary: Flatten DATA items into tables and assign literal ids.
 * Parameters:
 *  - lines: Kept line numbers to scan.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void CodeGenerator::cdCollectDataItems(const std::vector<int>& lines) {
    dataLiteralIds_.clear();
    dataIsString_.clear();
    dataNumValues_.clear();
    for (const int lineNum : lines) {
        cdCollectDataItemsFromLine(lineNum);
    }
}

} // namespace gwbasic
