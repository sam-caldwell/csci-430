// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: loadArrayElemAsDouble
 * Summary: Load an array element as double based on array numeric kind.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - arrayName: Array variable name (determines numeric kind).
 *  - elemPtrSSA: SSA name of the element pointer (ptr).
 * Returns:
 *  - std::string: SSA register name holding the double value.
 */
std::string CodeGenerator::loadArrayElemAsDouble(
    std::ostringstream& out,
    const std::string& arrayName,
    std::string_view elemPtrSSA) { // NOLINT(bugprone-easily-swappable-parameters)
    switch (numKindOf(arrayName)) {
        case NumKind::Int16: {
            std::string valReg = nextTemp();
            std::string dstReg = nextTemp();
            out << std::format("  {} = load i32, ptr {}", valReg, elemPtrSSA) << Symbols::LF
                << std::format("  {} = sitofp i32 {} to double", dstReg, valReg) << Symbols::LF;
            return dstReg;
        }
        case NumKind::Long32: {
            std::string valReg = nextTemp();
            std::string dstReg = nextTemp();
            out << std::format("  {} = load i64, ptr {}", valReg, elemPtrSSA) << Symbols::LF
                << std::format("  {} = sitofp i64 {} to double", dstReg, valReg) << Symbols::LF;
            return dstReg;
        }
        case NumKind::Single: {
            std::string valReg = nextTemp();
            std::string dstReg = nextTemp();
            out << std::format("  {} = load float, ptr {}", valReg, elemPtrSSA) << Symbols::LF
                << std::format("  {} = fpext float {} to double", dstReg, valReg) << Symbols::LF;
            return dstReg;
        }
        case NumKind::Double: default: {
            std::string dstReg = nextTemp();
            out << std::format("  {} = load double, ptr {}", dstReg, elemPtrSSA) << Symbols::LF;
            return dstReg;
        }
    }
}

} // namespace gwbasic
