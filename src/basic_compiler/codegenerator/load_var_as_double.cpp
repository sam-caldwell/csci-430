// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

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
            std::string l = nextTemp();
            std::string d = nextTemp();
            out << std::format("  {} = load i16, ptr {}", l, varAllocaName_[varName]) << Symbols::LF
                << std::format("  {} = sitofp i16 {} to double", d, l) << Symbols::LF;
            return d;
        }
        case NumKind::Long32: {
            std::string l = nextTemp();
            std::string d = nextTemp();
            out << std::format("  {} = load i32, ptr {}", l, varAllocaName_[varName]) << Symbols::LF
                << std::format("  {} = sitofp i32 {} to double", d, l) << Symbols::LF;
            return d;
        }
        case NumKind::Single: {
            std::string l = nextTemp();
            std::string d = nextTemp();
            out << std::format("  {} = load float, ptr {}", l, varAllocaName_[varName]) << Symbols::LF
                << std::format("  {} = fpext float {} to double", d, l) << Symbols::LF;
            return d;
        }
        case NumKind::Double: default: {
            std::string d = nextTemp();
            out << std::format("  {} = load double, ptr {}", d, varAllocaName_[varName]) << Symbols::LF;
            return d;
        }
    }
}

} // namespace gwbasic
