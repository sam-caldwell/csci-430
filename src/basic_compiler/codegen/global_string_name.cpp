// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <string>

namespace gwbasic {

/*
 * Function: globalStringName
 * Summary: Build the global symbol name for a string literal id.
 * Parameters:
 *  - id: String literal id.
 * Returns:
 *  - std::string: Symbol name like "@.str.<id>".
 */
std::string CodeGenerator::globalStringName(int literal_id) {
    return std::format("@.str.{}", literal_id);
}

} // namespace gwbasic
