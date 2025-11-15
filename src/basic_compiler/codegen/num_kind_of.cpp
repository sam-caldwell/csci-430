// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

/*
 * Function: numKindOf
 * Summary: Determine the numeric kind for a variable name.
 * Parameters:
 *  - name: Variable identifier.
 * Returns:
 *  - NumKind: Inferred numeric kind (Int16, Long32, Single, Double).
 */
CodeGenerator::NumKind CodeGenerator::numKindOf(const std::string& name) const {
    if (const auto iter = semNumericKinds_.find(name); iter != semNumericKinds_.end()) {
        return iter->second;
    }
    return NumKind::Single;
}

} // namespace gwbasic
