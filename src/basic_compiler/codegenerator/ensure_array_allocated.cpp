// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>
#include <format>

namespace gwbasic {

void CodeGenerator::ensureArrayAllocated(std::ostringstream& out, const std::string& name, int length) {
    if (auto it = arrayAllocaName_.find(name); it != arrayAllocaName_.end() && !it->second.empty()) return;
    std::string a = sanitizeLocal(name + std::string("_arr"));
    arrayAllocaName_[name] = a;
    std::string ty = arrayElemType(name);
    std::string ir = std::format("  {} = alloca [{} x {}]", a, length, ty);
    out << ir << Symbols::LF;
    log() << "line " << currentLine_ << " ArrayAlloc(" << name << ") -> " << ir << Symbols::LF;
}

} // namespace gwbasic
