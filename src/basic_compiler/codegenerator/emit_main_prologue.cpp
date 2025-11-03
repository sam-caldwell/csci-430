// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitMainPrologue(std::ostringstream& out) {
    /*
     * Function: CodeGenerator::emitMainPrologue
     * Inputs:
     *  - out: IR output stream
     * Outputs:
     *  - void
     * Theory of operation:
     *  - Starts the main function, allocates all discovered variables on the
     *    stack, initializes them to 0.0, and branches to the first line label
     *    or returns 0 if the program has no lines.
     */
    out << "define i32 @main() {" << STR_LF
        << "entry:" << STR_LF;
    for (const auto& v : variables_) {
        std::string a = "%"; a += v;
        varAllocaName_[v] = a;
        std::string i1 = "  "; i1 += a; i1 += " = alloca double";
        std::string i2 = "  store double 0.0, ptr "; i2 += a;
        out << i1 << STR_LF
            << i2 << STR_LF;
        log() << "line 0 VarAlloc(" << v << ") -> " << i1 << CH_LF;
        log() << "line 0 InitZero(" << v << ") -> " << i2 << CH_LF;
    }
    if (!lineNumbers_.empty()) { std::string br = "  br label %"; br += lineLabelName(lineNumbers_.front()); out << br << STR_LF; log() << "entry -> " << br << CH_LF; }
    else { out << "  ret i32 0" << STR_LF; out << "}" << STR_LF; }
}

} // namespace gwbasic
