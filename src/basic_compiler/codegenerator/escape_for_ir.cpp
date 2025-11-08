// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

std::string CodeGenerator::escapeForIR(const std::string& s) {
    /*
     * Function: CodeGenerator::escapeForIR
     * Inputs:
     *  - s: raw string literal contents
     * Outputs:
     *  - std::string: string with LLVM IR escape sequences for special bytes
     * Theory of operation:
     *  - Walks each byte, emitting printable ASCII as-is and percent-style
     *    hex escapes for control/non-ASCII; uses predefined escapes for
     *    common characters like backslash and quotes.
     */
    std::string out;
    for (const unsigned char c : s) {
        CodeGenerator::appendEscapedByte(out, c);
    }
    return out;
}

} // namespace gwbasic
