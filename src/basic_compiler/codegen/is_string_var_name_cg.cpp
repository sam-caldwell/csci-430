// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <string>

namespace gwbasic {

/*
 * Function: isStringVarNameCG
 * Summary: Determine if a variable name denotes a string type in codegen.
 * Parameters:
 *  - name: Variable identifier.
 * Returns:
 *  - bool: True if name ends with '$' or is known as a string variable.
 */
bool CodeGenerator::isStringVarNameCG(const std::string& name) const {
    if (!name.empty() && name.back() == Symbols::DOLLARSIGN.first()) {
        return true;
    }
    return semStringVariables_.contains(name);
}

} // namespace gwbasic
