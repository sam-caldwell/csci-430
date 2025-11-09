// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

void CodeGenerator::storeNumberToArrayElem(std::ostringstream& out,
                                           const std::string& arrayName,
                                           const std::string& elemPtrSSA,
                                           const std::string& doubleValSSA) {
    switch (numKindOf(arrayName)) {
        case NumKind::Int16: {
            std::string cvt = nextTemp();
            { std::string ir = std::format("  {} = fptosi double {} to i32", cvt, doubleValSSA); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i32 {}, ptr {}", cvt, elemPtrSSA); out << ir << Symbols::LF; }
            break;
        }
        case NumKind::Long32: {
            std::string cvt = nextTemp();
            { std::string ir = std::format("  {} = fptosi double {} to i64", cvt, doubleValSSA); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i64 {}, ptr {}", cvt, elemPtrSSA); out << ir << Symbols::LF; }
            break;
        }
        case NumKind::Single: {
            std::string cvt = nextTemp();
            { std::string ir = std::format("  {} = fptrunc double {} to float", cvt, doubleValSSA); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store float {}, ptr {}", cvt, elemPtrSSA); out << ir << Symbols::LF; }
            break;
        }
        case NumKind::Double: {
            { std::string ir = std::format("  store double {}, ptr {}", doubleValSSA, elemPtrSSA); out << ir << Symbols::LF; }
            break;
        }
    }
}

} // namespace gwbasic

