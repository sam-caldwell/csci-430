// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ResumeStmt.h" // IWYU pragma: keep
#include <vector>

namespace gwbasic {

/*
 * Function: cdFindResumeEndIdx
 * Summary: Find the index of the first line at/after startIdx containing RESUME.
 * Parameters:
 *  - lines: Kept line numbers in order.
 *  - startIdx: Starting index into lines.
 * Returns:
 *  - int: Index of the first line containing RESUME, or -1 if none.
 */
// NOLINTNEXTLINE(readability-function-size,readability-convert-member-functions-to-static)
int CodeGenerator::cdFindResumeEndIdx(const std::vector<int>& lines, const int startIdx) const {
    for (int j = startIdx; j < static_cast<int>(lines.size()); ++j) {
        const auto* const linePtr = lineMap_.at(lines[j]);
        if (linePtr == nullptr) {
            continue;
        }
        for (const auto& stmtNode : linePtr->statements) {
            if (isa<const ResumeStmt>(stmtNode.get())) {
                return j;
            }
        }
    }
    return -1;
}

} // namespace gwbasic
