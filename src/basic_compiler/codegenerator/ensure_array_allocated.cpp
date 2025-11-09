// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

void CodeGenerator::ensureArrayAllocated(std::ostringstream& out, const std::string& name, int length) {
    if (auto foundIt = arrayAllocaName_.find(name); foundIt != arrayAllocaName_.end() && !foundIt->second.empty()) {
        return;
    }
    std::string allocName = sanitizeLocal(name + std::string("_arr"));
    arrayAllocaName_[name] = allocName;
    std::string elemType = arrayElemType(name);
    const std::string irLine = std::format("  {} = alloca [{} x {}]", allocName, length, elemType);
    out << irLine << Symbols::LF;
    log() << "line " << currentLine_ << " ArrayAlloc(" << name << ") -> " << irLine << Symbols::LF;
}

} // namespace gwbasic
