// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

/*
 * Function: resumeLabelName
 * Summary: Build the label used for RESUME at a specific statement.
 * Parameters:
 *  - ln: Source line number.
 *  - stmtIndex: Statement index within the line.
 * Returns:
 *  - std::string: Label like "resume_l<ln>_<stmtIndex>".
 */
std::string CodeGenerator::resumeLabelName(int ln, int stmtIndex) {
    return std::format("resume_l{}_{}", ln, stmtIndex);
}

} // namespace gwbasic
