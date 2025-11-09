// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

std::string CodeGenerator::arrayElemType(const std::string& name) const {
    switch (numKindOf(name)) {
        case NumKind::Int16: return "i32";   // Integer arrays map to i32
        case NumKind::Long32: return "i64";  // Long arrays map to i64
        case NumKind::Single: return "float"; // Single arrays are true float
        case NumKind::Double: default: return "double";
    }
}

} // namespace gwbasic

