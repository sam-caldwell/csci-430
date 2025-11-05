// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <sstream>
#include <format>

namespace gwbasic {

void CodeGenerator::resetVar(std::ostringstream& out, const std::string& name) {
    const std::string& dst = varAllocaName_[name];
    if (isStringVarNameCG(name)) {
        std::string ir = std::format("  store ptr null, ptr {}", dst);
        out << ir << Symbols::LF;
        return;
    }
    switch (numKindOf(name)) {
        case NumKind::Int16: { std::string ir = std::format("  store i16 0, ptr {}", dst); out << ir << Symbols::LF; break; }
        case NumKind::Long32:{ std::string ir = std::format("  store i32 0, ptr {}", dst); out << ir << Symbols::LF; break; }
        case NumKind::Single:{ std::string ir = std::format("  store float 0.0, ptr {}", dst); out << ir << Symbols::LF; break; }
        case NumKind::Double:{ std::string ir = std::format("  store double 0.0, ptr {}", dst); out << ir << Symbols::LF; break; }
    }
}

} // namespace gwbasic

