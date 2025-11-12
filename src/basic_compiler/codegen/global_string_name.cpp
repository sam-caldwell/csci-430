// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>
#include <format>

namespace gwbasic {

/*
 * Function: globalStringName
 * Summary: Build the global symbol name for a string literal id.
 * Parameters:
 *  - id: String literal id.
 * Returns:
 *  - std::string: Symbol name like "@.str.<id>".
 */
std::string CodeGenerator::globalStringName(int id) {
    return std::format("@.str.{}", id);
}

} // namespace gwbasic
