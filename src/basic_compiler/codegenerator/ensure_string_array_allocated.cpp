// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>
#include <format>

namespace gwbasic {

/*
 * Function: ensureStringArrayAllocated
 * Summary: Ensure a string array allocation exists for a named variable.
 * Parameters:
 *  - out: IR output stream to append allocation instructions.
 *  - name: Array variable name.
 *  - length: Number of elements to allocate.
 * Returns:
 *  - void
 */
void CodeGenerator::ensureStringArrayAllocated(std::ostringstream& out, const std::string& name, int length) {
    if (auto it = arrayAllocaName_.find(name); it != arrayAllocaName_.end() && !it->second.empty()) return;
    std::string a = sanitizeLocal(name + std::string("_arr"));
    arrayAllocaName_[name] = a;
    std::string ir = std::format("  {} = alloca [{} x ptr]", a, length);
    out << ir << Symbols::LF;
    log() << "line " << currentLine_ << " StringArrayAlloc(" << name << ") -> " << ir << Symbols::LF;
}

} // namespace gwbasic
