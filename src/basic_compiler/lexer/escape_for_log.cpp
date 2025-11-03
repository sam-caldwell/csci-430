// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <cstdio>

namespace gwbasic {

/*
 * Function: Lexer::escapeForLog
 * Inputs:
 *  - s: Raw string to escape for logging
 * Outputs:
 *  - std::string: Escaped string with common control chars rendered
 * Theory of operation:
 *  - Iterates characters, escapes backslash, quotes, and control codes as
 *    C-style sequences; leaves printable ASCII unchanged.
 */
std::string Lexer::escapeForLog(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (const unsigned char ch : s) {
        switch (ch) {
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            case '"':  out += "\\\""; break;
            default:
                if (ch < 0x20 || ch == 0x7F) {
                    char buf[5];
                    std::snprintf(buf, sizeof(buf), "\\x%02X", ch);
                    out += buf;
                } else {
                    out.push_back(static_cast<char>(ch));
                }
        }
    }
    return out;
}

} // namespace gwbasic
