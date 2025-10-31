// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

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
        switch (c) {
            case '\\': out += "\\5C"; break;
            case '"': out += "\\22"; break;
            case '\n': out += "\\0A"; break;
            case '\t': out += "\\09"; break;
            case '\r': out += "\\0D"; break;
            default:
                if (c >= 32 && c < 127) out.push_back(static_cast<char>(c));
                else { char buf[5]; std::snprintf(buf, sizeof(buf), "\\%02X", c); out += buf; }
                break;
        }
    }
    return out;
}

} // namespace gwbasic
