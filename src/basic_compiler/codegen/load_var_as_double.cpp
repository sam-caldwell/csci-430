// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: loadVarAsDouble
 * Summary: Load a typed variable as a double value.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - varName: Variable identifier to load.
 * Returns:
 *  - std::string: SSA register name holding the double value.
 */
std::string CodeGenerator::loadVarAsDouble(std::ostringstream& out, const std::string& varName) {
    switch (numKindOf(varName)) {
        case NumKind::Int16: {
            std::string loaded = nextTemp();
            std::string dblReg = nextTemp();
            out << std::format("  {} = load i16, ptr {}", loaded, varAllocaName_[varName]) << Symbols::LF
                << std::format("  {} = sitofp i16 {} to double", dblReg, loaded) << Symbols::LF;
            return dblReg;
        }
        case NumKind::Long32: {
            std::string loaded = nextTemp();
            std::string dblReg = nextTemp();
            out << std::format("  {} = load i32, ptr {}", loaded, varAllocaName_[varName]) << Symbols::LF
                << std::format("  {} = sitofp i32 {} to double", dblReg, loaded) << Symbols::LF;
            return dblReg;
        }
        case NumKind::Single: {
            std::string loaded = nextTemp();
            std::string dblReg = nextTemp();
            out << std::format("  {} = load float, ptr {}", loaded, varAllocaName_[varName]) << Symbols::LF
                << std::format("  {} = fpext float {} to double", dblReg, loaded) << Symbols::LF;
            return dblReg;
        }
        case NumKind::Double: default: {
            std::string dblReg = nextTemp();
            out << std::format("  {} = load double, ptr {}", dblReg, varAllocaName_[varName]) << Symbols::LF;
            return dblReg;
        }
    }
}

} // namespace gwbasic
