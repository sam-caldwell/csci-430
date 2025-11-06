// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>
#include <format>

namespace gwbasic {

void CodeGenerator::ensureStringArrayAllocated(std::ostringstream& out, const std::string& name, int length) {
    if (auto it = arrayAllocaName_.find(name); it != arrayAllocaName_.end() && !it->second.empty()) return;
    std::string a = "%"; a += name; a += "_arr";
    arrayAllocaName_[name] = a;
    std::string ir = std::format("  {} = alloca [{} x ptr]", a, length);
    out << ir << Symbols::LF;
    log() << "line " << currentLine_ << " StringArrayAlloc(" << name << ") -> " << ir << Symbols::LF;
}

} // namespace gwbasic

