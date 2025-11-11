// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <cstddef>
#include <vector>

namespace gwbasic {

/*
 * Function: cdFindLineIndex
 * Summary: Find index of a line number within 'lines'.
 * Parameters:
 *  - lines: Ordered line numbers to search.
 *  - line: Line number to find.
 * Returns:
 *  - int: Index within 'lines' or -1 if missing.
 */
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
int CodeGenerator::cdFindLineIndex(const std::vector<int>& lines, const int line) const {
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i] == line) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

} // namespace gwbasic
