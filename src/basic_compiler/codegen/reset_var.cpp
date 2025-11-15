// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: resetVar
 * Summary: Reset a variable to its default zero/null value.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - name: Variable identifier.
 * Returns:
 *  - void
 */
void CodeGenerator::resetVar(std::ostringstream& out, const std::string& name) {
    const std::string& dst = varAllocaName_[name];
    if (isStringVarNameCG(name)) {
        out << std::format("  store ptr null, ptr {}", dst) << Symbols::LF;
        return;
    }
    switch (numKindOf(name)) {
        case NumKind::Int16: {
            out << std::format("  store i16 0, ptr {}", dst) << Symbols::LF;
            break;
        }
        case NumKind::Long32:{
            out << std::format("  store i32 0, ptr {}", dst) << Symbols::LF;
            break;
        }
        case NumKind::Single:{
            out << std::format("  store float 0.0, ptr {}", dst) << Symbols::LF;
            break;
        }
        case NumKind::Double:{
            out << std::format("  store double 0.0, ptr {}", dst) << Symbols::LF;
            break;
        }
    }
}

} // namespace gwbasic
