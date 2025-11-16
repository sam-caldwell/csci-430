// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: ensureArrayAllocated
 * Summary: Ensure a string/array allocation exists for a named variable.
 * Parameters:
 *  - out: IR output stream to append allocation instructions.
 *  - name: Array variable name.
 *  - length: Number of elements to allocate.
 * Returns:
 *  - void
 */
void CodeGenerator::ensureArrayAllocated(std::ostringstream& out, const std::string& name, int length) {
    if (auto foundIt = arrayAllocaName_.find(name); foundIt != arrayAllocaName_.end() && !foundIt->second.empty()) {
        return;
    }
    std::string allocName = sanitizeLocal(std::format("{}_arr", name));
    arrayAllocaName_[name] = allocName;
    std::string elemType = arrayElemType(name);
    const std::string irLine = std::format("  {} = alloca [{} x {}]", allocName, length, elemType);
    out << irLine << Symbols::LF;
}

} // namespace gwbasic
