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
    out << "define i32 @main() {" << Symbols::LF
        << "entry:" << Symbols::LF;
    for (const auto& v : variables_) {
        // Allocate and initialize each variable using typed storage
        ensureVarAllocated(out, v);
    }
    if (!lineNumbers_.empty()) { std::string br = "  br label %"; br += lineLabelName(lineNumbers_.front()); out << br << Symbols::LF; log() << "entry -> " << br << Symbols::LF; }
    else { out << "  ret i32 0" << Symbols::LF; out << "}" << Symbols::LF; }
}

} // namespace gwbasic
