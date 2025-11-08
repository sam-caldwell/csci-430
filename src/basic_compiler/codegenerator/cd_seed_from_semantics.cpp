// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdSeedFromSemantics
 * Purpose: If provided, seed variables/strings/common from prior semantic analysis.
 */
void CodeGenerator::cdSeedFromSemantics() {
    if (!semProvided_) return;
    variables_ = semVariables_;
    commonVariables_ = semCommonVariables_;
    strLiteralId_.clear(); strCounter_ = 0;
    for (const auto& s : semStrings_) if (!strLiteralId_.contains(s)) strLiteralId_[s] = strCounter_++;
}

} // namespace gwbasic

