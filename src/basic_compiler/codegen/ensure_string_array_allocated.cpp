// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>

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
    if (auto iter = arrayAllocaName_.find(name); iter != arrayAllocaName_.end() && !iter->second.empty()) {
        return;
    }
    std::string localName = sanitizeLocal(std::format("{}_arr", name));
    arrayAllocaName_[name] = localName;
    const std::string allocIr = std::format("  {} = alloca [{} x ptr]", localName, length);
    out << allocIr << Symbols::LF;
}

} // namespace gwbasic
