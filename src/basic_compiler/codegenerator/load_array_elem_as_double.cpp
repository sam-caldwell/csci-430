// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

std::string CodeGenerator::loadArrayElemAsDouble(std::ostringstream& out,
                                                 const std::string& arrayName,
                                                 const std::string& elemPtrSSA) {
    switch (numKindOf(arrayName)) {
        case NumKind::Int16: {
            std::string v = nextTemp();
            { std::string ir = std::format("  {} = load i32, ptr {}", v, elemPtrSSA); out << ir << Symbols::LF; }
            std::string d = nextTemp();
            { std::string ir = std::format("  {} = sitofp i32 {} to double", d, v); out << ir << Symbols::LF; }
            return d;
        }
        case NumKind::Long32: {
            std::string v = nextTemp();
            { std::string ir = std::format("  {} = load i64, ptr {}", v, elemPtrSSA); out << ir << Symbols::LF; }
            std::string d = nextTemp();
            { std::string ir = std::format("  {} = sitofp i64 {} to double", d, v); out << ir << Symbols::LF; }
            return d;
        }
        case NumKind::Single: {
            std::string v = nextTemp();
            { std::string ir = std::format("  {} = load float, ptr {}", v, elemPtrSSA); out << ir << Symbols::LF; }
            std::string d = nextTemp();
            { std::string ir = std::format("  {} = fpext float {} to double", d, v); out << ir << Symbols::LF; }
            return d;
        }
        case NumKind::Double: default: {
            std::string d = nextTemp();
            { std::string ir = std::format("  {} = load double, ptr {}", d, elemPtrSSA); out << ir << Symbols::LF; }
            return d;
        }
    }
}

} // namespace gwbasic

