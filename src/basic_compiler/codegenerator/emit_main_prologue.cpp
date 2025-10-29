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
    out << "define i32 @main() {\n"
        << "entry:\n";
    for (const auto& v : variables_) {
        std::string a = "%"; a += v;
        varAllocaName_[v] = a;
        std::string i1 = "  "; i1 += a; i1 += " = alloca double";
        std::string i2 = "  store double 0.0, ptr "; i2 += a;
        out << i1 << "\n"
            << i2 << "\n";
        { std::ostringstream m; m << "line 0 VarAlloc(" << v << ") -> " << i1; log(m.str()); }
        { std::ostringstream m; m << "line 0 InitZero(" << v << ") -> " << i2; log(m.str()); }
    }
    if (!lineNumbers_.empty()) { std::string br = "  br label %"; br += lineLabelName(lineNumbers_.front()); out << br << "\n"; { std::ostringstream m; m << "entry -> " << br; log(m.str()); } }
    else { out << "  ret i32 0\n"; out << "}\n"; }
}

} // namespace gwbasic
