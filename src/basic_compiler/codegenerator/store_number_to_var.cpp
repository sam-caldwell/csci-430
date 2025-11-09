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
        using enum gwbasic::CodeGenerator::NumKind;
        case Int16: {
            std::string iv = nextTemp();
            out << std::format("  {} = fptosi double {} to i16", iv, doubleValSSA) << Symbols::LF
                << std::format("  store i16 {}, ptr {}", iv, dst) << Symbols::LF;
            break;
        }
        case Long32: {
            std::string iv = nextTemp();
            out << std::format("  {} = fptosi double {} to i32", iv, doubleValSSA) << Symbols::LF
                << std::format("  store i32 {}, ptr {}", iv, dst) << Symbols::LF;
            break;
        }
        case Single: {
            std::string fv = nextTemp();
            out << std::format("  {} = fptrunc double {} to float", fv, doubleValSSA) << Symbols::LF
                << std::format("  store float {}, ptr {}", fv, dst) << Symbols::LF;
            break;
        }
        case Double: {
            out << std::format("  store double {}, ptr {}", doubleValSSA, dst) << Symbols::LF;
            break;
        }
    }
}

} // namespace gwbasic
