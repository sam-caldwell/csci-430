// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <vector>

namespace gwbasic {

/*
 * Function: cdScanRndAndStop
 * Summary: Scan kept lines to determine helper emission needs (RND, STOP).
 * Parameters:
 *  - lines: Kept line numbers to scan.
 * Returns:
 *  - void
 */
void CodeGenerator::cdScanRndAndStop(const std::vector<int>& lines) {
    for (const int lineNum : lines) {
        const auto* lptr = lineMap_[lineNum];
        if (lptr == nullptr) { continue; }
        for (const auto& stmtNode : lptr->statements) {
            scanStmtForRnd(stmtNode.get());
            scanStmtForStop(stmtNode.get());
        }
    }
}

} // namespace gwbasic
