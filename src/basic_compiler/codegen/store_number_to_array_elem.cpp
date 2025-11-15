// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: storeNumberToArrayElem
 * Summary: Store a double value into an array element of typed kind.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - arrayName: Array variable name (determines numeric kind).
 *  - elemPtrSSA: SSA name of the element pointer (ptr).
 *  - doubleValSSA: SSA name of the source double value.
 * Returns:
 *  - void
 */
void CodeGenerator::storeNumberToArrayElem(std::ostringstream& out,
                                           const std::string& arrayName,
                                           std::string_view elemPtrSSA,
                                           const std::string& doubleValSSA) {

    switch (numKindOf(arrayName)) {
        using enum gwbasic::CodeGenerator::NumKind;
        case Int16: {
            std::string cvt = nextTemp();
            out << std::format("  {} = fptosi double {} to i32", cvt, doubleValSSA) << Symbols::LF
                << std::format("  store i32 {}, ptr {}", cvt, elemPtrSSA) << Symbols::LF;
            break;
        }
        case Long32: {
            std::string cvt = nextTemp();
            out << std::format("  {} = fptosi double {} to i64", cvt, doubleValSSA) << Symbols::LF
                << std::format("  store i64 {}, ptr {}", cvt, elemPtrSSA) << Symbols::LF;
            break;
        }
        case Single: {
            std::string cvt = nextTemp();
            out << std::format("  {} = fptrunc double {} to float", cvt, doubleValSSA) << Symbols::LF
                << std::format("  store float {}, ptr {}", cvt, elemPtrSSA) << Symbols::LF;
            break;
        }
        case Double: {
            out << std::format("  store double {}, ptr {}", doubleValSSA, elemPtrSSA) << Symbols::LF;
            break;
        }
    }
}

} // namespace gwbasic
