// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"
#include <cstddef>

namespace gwbasic {

/*
 * Function: Lexer::escapeForLog
 * Summary:
 *  Escape control characters for readable logging using C-style sequences.
 * Parameters:
 *  - s: Raw input string to escape for logging
 * Returns:
 *  - std::string: Escaped string with control characters rendered
 */
std::string Lexer::escapeForLog(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (const unsigned char ch : s) {
        switch (ch) {
            case '\\': out += R"(\\)"; break;
            case '\n': out += R"(\n)"; break;
            case '\r': out += R"(\r)"; break;
            case '\t': out += R"(\t)"; break;
            case '"':  out += R"(\")";
                break;
            default:
                if (ch < Symbols::SPACE.first() || ch == Symbols::DEL.first()) {
                    static constexpr char HEX[] = "0123456789ABCDEF";
                    out += "\\x";
                    const auto b = static_cast<std::byte>(ch);
                    const auto hi = std::to_integer<unsigned int>(b >> 4);
                    const auto lo = std::to_integer<unsigned int>(b & std::byte{0x0F});
                    out.push_back(HEX[hi]);
                    out.push_back(HEX[lo]);
                } else {
                    out.push_back(static_cast<char>(ch));
                }
        }
    }
    return out;
}

} // namespace gwbasic
