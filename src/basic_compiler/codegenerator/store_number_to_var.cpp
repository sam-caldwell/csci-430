// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <sstream>
#include <format>

namespace gwbasic {

/*
 * Function: storeNumberToVar
 * Summary: Store a computed double value into a typed variable slot.
 * Parameters:
 *  - out: IR output stream to append store instructions.
 *  - name: Target variable name.
 *  - doubleValSSA: SSA register holding the double value to store.
 * Returns:
 *  - void
 */
void CodeGenerator::storeNumberToVar(std::ostringstream& out, const std::string& name, const std::string& doubleValSSA) {
    const std::string& dst = varAllocaName_[name];
    switch (numKindOf(name)) {
        case NumKind::Int16: {
            std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i16", iv, doubleValSSA); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i16 {}, ptr {}", iv, dst); out << ir << Symbols::LF; }
            break;
        }
        case NumKind::Long32: {
            std::string iv = nextTemp(); { std::string ir = std::format("  {} = fptosi double {} to i32", iv, doubleValSSA); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 {}, ptr {}", iv, dst); out << ir << Symbols::LF; }
            break;
        }
        case NumKind::Single: {
            std::string fv = nextTemp(); { std::string ir = std::format("  {} = fptrunc double {} to float", fv, doubleValSSA); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store float {}, ptr {}", fv, dst); out << ir << Symbols::LF; }
            break;
        }
        case NumKind::Double: {
            { std::string ir = std::format("  store double {}, ptr {}", doubleValSSA, dst); out << ir << Symbols::LF; }
            break;
        }
    }
}

} // namespace gwbasic
