// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ResumeStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdFindResumeEndIdx
 * Purpose: Search forward from startIdx for the first line containing RESUME.
 */
int CodeGenerator::cdFindResumeEndIdx(const std::vector<int>& lines, int startIdx) const {
    for (int j = startIdx; j < static_cast<int>(lines.size()); ++j) {
        const auto* lp = lineMap_.at(lines[j]);
        if (!lp) continue;
        for (const auto& st : lp->statements) if (isa<const ResumeStmt>(st.get())) return j;
    }
    return -1;
}

} // namespace gwbasic

