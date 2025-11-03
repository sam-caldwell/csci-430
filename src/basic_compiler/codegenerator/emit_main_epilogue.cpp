// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitMainEpilogue(std::ostringstream& out) {
    /*
     * Function: CodeGenerator::emitMainEpilogue
     * Inputs:
     *  - out: IR output stream
     * Outputs:
     *  - void
     * Theory of operation:
     *  - Emits the exit label and returns 0 to finish main.
     */
    out << "exit:" << STR_LF;
    out << "  ret i32 0" << STR_LF;
    out << "}" << STR_LF;
}

} // namespace gwbasic
