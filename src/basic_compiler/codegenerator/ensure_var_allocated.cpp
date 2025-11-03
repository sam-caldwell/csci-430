// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>
#include <format>

namespace gwbasic {

void CodeGenerator::ensureVarAllocated(std::ostringstream& out, const std::string& name) {
    /*
     * Function: CodeGenerator::ensureVarAllocated
     * Inputs:
     *  - out: IR stream (insertion point)
     *  - name: variable identifier
     * Outputs:
     *  - void (may emit an alloca+store 0.0)
     * Theory of operation:
     *  - Checks for an existing alloca mapping; if absent, emits an alloca
     *    of type double and zero-initializes it.
     */
    if (auto it = varAllocaName_.find(name); it != varAllocaName_.end() && !it->second.empty()) return;
    std::string a = "%"; a += name;
    varAllocaName_[name] = a;
    const bool isStr = (!name.empty() && name.back() == CH_DOLLARSIGN);
    if (isStr) {
        std::string ir1 = std::format("  {} = alloca ptr", a);
        out << ir1 << STR_LF; log() << "line " << currentLine_ << " VarAllocStr(" << name << ") -> " << ir1 << CH_LF;
        std::string ir2 = std::format("  store ptr null, ptr {}", a);
        out << ir2 << STR_LF; log() << "line " << currentLine_ << " InitNullStr(" << name << ") -> " << ir2 << CH_LF;
    } else {
        std::string ir1 = std::format("  {} = alloca double", a);
        out << ir1 << STR_LF; log() << "line " << currentLine_ << " VarAlloc(" << name << ") -> " << ir1 << CH_LF;
        std::string ir2 = std::format("  store double 0.0, ptr {}", a);
        out << ir2 << STR_LF; log() << "line " << currentLine_ << " InitZero(" << name << ") -> " << ir2 << CH_LF;
    }
}

} // namespace gwbasic
