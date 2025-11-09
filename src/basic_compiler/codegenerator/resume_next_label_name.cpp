// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

/*
 * Function: resumeNextLabelName
 * Summary: Build the label used for RESUME NEXT at a specific statement.
 * Parameters:
 *  - ln: Source line number.
 *  - stmtIndex: Statement index within the line.
 * Returns:
 *  - std::string: Label like "resume_next_l<ln>_<stmtIndex>".
 */
std::string CodeGenerator::resumeNextLabelName(const int ln, const int stmtIndex) {
    return std::format("resume_next_{}_{}", ln, stmtIndex);
}

} // namespace gwbasic
