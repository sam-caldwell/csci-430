// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdScanRndAndStop
 * Purpose: Scan kept lines to determine helper emission needs (RND, STOP).
 */
void CodeGenerator::cdScanRndAndStop(const std::vector<int>& lines) {
    for (int ln : lines) {
        const auto* lptr = lineMap_[ln];
        if (!lptr) continue;
        for (const auto& st : lptr->statements) {
            scanStmtForRnd(st.get());
            scanStmtForStop(st.get());
        }
    }
}

} // namespace gwbasic

