// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

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
    {
        std::string ir1 = "  "; ir1 += a; ir1 += " = alloca double";
        out << ir1 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " VarAlloc(" << name << ") -> " << ir1; log(m.str()); }
    }
    {
        std::string ir2 = "  store double 0.0, ptr "; ir2 += a;
        out << ir2 << "\n"; { std::ostringstream m; m << "line " << currentLine_ << " InitZero(" << name << ") -> " << ir2; log(m.str()); }
    }
}

} // namespace gwbasic
