// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: cdSeedFromSemantics
 * Summary: Seed variables/strings/common sets from semantic analysis results.
 * Parameters:
 *  - (none)
 * Returns:
 *  - void
 */
void CodeGenerator::cdSeedFromSemantics() {
    if (!semProvided_) return;
    variables_ = semVariables_;
    commonVariables_ = semCommonVariables_;
    strLiteralId_.clear(); strCounter_ = 0;
    for (const auto& s : semStrings_)
        if (!strLiteralId_.contains(s))
            strLiteralId_[s] = strCounter_++;
}

} // namespace gwbasic
