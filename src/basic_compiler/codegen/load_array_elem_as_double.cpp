// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

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
std::string CodeGenerator::loadArrayElemAsDouble(std::ostringstream& out,
                                                 const std::string& arrayName,
                                                 const std::string& elemPtrSSA) {
    switch (numKindOf(arrayName)) {
        case NumKind::Int16: {
            std::string v = nextTemp();
            std::string d = nextTemp();
            out << std::format("  {} = load i32, ptr {}", v, elemPtrSSA) << Symbols::LF
                << std::format("  {} = sitofp i32 {} to double", d, v) << Symbols::LF;
            return d;
        }
        case NumKind::Long32: {
            std::string v = nextTemp();
            std::string d = nextTemp();
            out << std::format("  {} = load i64, ptr {}", v, elemPtrSSA) << Symbols::LF
                << std::format("  {} = sitofp i64 {} to double", d, v) << Symbols::LF;
            return d;
        }
        case NumKind::Single: {
            std::string v = nextTemp();
            std::string d = nextTemp();
            out << std::format("  {} = load float, ptr {}", v, elemPtrSSA) << Symbols::LF
                << std::format("  {} = fpext float {} to double", d, v) << Symbols::LF;
            return d;
        }
        case NumKind::Double: default: {
            std::string d = nextTemp();
            out << std::format("  {} = load double, ptr {}", d, elemPtrSSA) << Symbols::LF;
            return d;
        }
    }
}

} // namespace gwbasic
