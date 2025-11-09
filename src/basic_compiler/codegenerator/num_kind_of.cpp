// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

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
    auto it = semNumericKinds_.find(name);
    if (it != semNumericKinds_.end()) {
        return it->second;
    }
    return NumKind::Single;
}

} // namespace gwbasic
