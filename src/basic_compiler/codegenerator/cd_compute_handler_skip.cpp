// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/ResumeStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdComputeHandlerSkipAfter
 * Purpose: Determine the line to skip to after an ON ERROR handler when not in handler context.
 */
void CodeGenerator::cdComputeHandlerSkipAfter(const std::vector<int>& lines) {
    handlerSkipAfter_.clear();
    std::set<int> trapTargets;
    for (int ln : lines) {
        const auto* lptr = lineMap_[ln]; if (!lptr) continue;
        for (const auto& st : lptr->statements) if (const auto oeg = dyn_cast<const OnErrorGotoStmt>(st.get())) if (oeg->targetLine > 0) trapTargets.insert(oeg->targetLine);
    }
    for (int t : trapTargets) {
        int startIdx = -1;
        for (size_t i = 0; i < lines.size(); ++i) if (lines[i] == t) { startIdx = static_cast<int>(i); break; }
        if (startIdx < 0) continue;
        int endIdx = -1;
        for (int j = startIdx; j < static_cast<int>(lines.size()); ++j) {
            const auto* lp = lineMap_[lines[j]]; if (!lp) continue;
            bool hasResume = false;
            for (const auto& st : lp->statements) if (isa<const ResumeStmt>(st.get())) { hasResume = true; break; }
            if (hasResume) { endIdx = j; break; }
        }
        int skipTo = -1;
        if (endIdx >= 0) {
            if (endIdx + 1 < static_cast<int>(lines.size())) skipTo = lines[endIdx + 1];
        } else {
            if (startIdx + 1 < static_cast<int>(lines.size())) skipTo = lines[startIdx + 1];
        }
        handlerSkipAfter_[t] = skipTo;
    }
}

} // namespace gwbasic

