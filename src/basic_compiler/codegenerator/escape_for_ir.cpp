// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: escapeForIR
 * Summary: Escape a raw string for LLVM IR constant syntax.
 * Parameters:
 *  - s: Raw string literal contents.
 * Returns:
 *  - std::string: IR-escaped string with byte escapes for special chars.
 */
std::string CodeGenerator::escapeForIR(const std::string& s) {
    std::string out;
    for (const unsigned char c : s) {
        CodeGenerator::appendEscapedByte(out, c);
    }
    return out;
}

} // namespace gwbasic
