// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: isStringArrayNameCG
 * Summary: Determine if a variable name denotes a string array in codegen.
 * Parameters:
 *  - name: Array variable identifier.
 * Returns:
 *  - bool: True if the corresponding scalar name would be string.
 */
bool CodeGenerator::isStringArrayNameCG(const std::string& name) const {
    return isStringVarNameCG(name);
}

} // namespace gwbasic
