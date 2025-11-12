// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <string>

namespace gwbasic {

/*
 * Function: nextTemp
 * Summary: Generate a fresh SSA temporary name.
 * Parameters:
 *  - (none)
 * Returns:
 *  - std::string: Name like "%t<counter>".
 */
std::string CodeGenerator::nextTemp() {
    return std::format("%t{}", ++tempCounter_);
}

} // namespace gwbasic
