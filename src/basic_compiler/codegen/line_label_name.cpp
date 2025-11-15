// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <string>

namespace gwbasic {

/*
 * Function: lineLabelName
 * Summary: Build the canonical label for a BASIC source line.
 * Parameters:
 *  - ln: Source line number.
 * Returns:
 *  - std::string: Label name prefixing the line number (e.g., "line100").
 */
std::string CodeGenerator::lineLabelName(int line_num) {
    return std::format("line{}", line_num);
}

} // namespace gwbasic
