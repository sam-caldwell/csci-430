// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

/*
 * Function: arrayElemType
 * Summary: Determine the LLVM IR element type string for an array.
 * Parameters:
 *  - name: Array variable identifier.
 * Returns:
 *  - std::string: IR type name (e.g., "double", "float", "i32", "i64").
 */
std::string CodeGenerator::arrayElemType(const std::string& name) const {
    switch (numKindOf(name)) {
        using enum gwbasic::CodeGenerator::NumKind;
        case Int16: return "i32";   // Integer arrays map to i32
        case Long32: return "i64";  // Long arrays map to i64
        case Single: return "float"; // Single arrays are true float
        case Double: default: return "double";
    }
}

} // namespace gwbasic
