// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"
#include <cstddef>
#include <string>

namespace gwbasic {

/*
 * Function: Lexer::escapeForLog
 * Summary:
 *  Escape control characters for readable logging using C-style sequences.
 * Parameters:
 *  - text: Raw input string to escape for logging
 * Returns:
 *  - std::string: Escaped string with control characters rendered
 */
std::string Lexer::escapeForLog(const std::string& text) { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    std::string out;
    out.reserve(text.size());
    for (const unsigned char chr : text) {
        switch (chr) {
            case '\\': out += R"(\\)"; break;
            case '\n': out += R"(\n)"; break;
            case '\r': out += R"(\r)"; break;
            case '\t': out += R"(\t)"; break;
            case '"':  out += R"(\")";
                break;
            default:
                if (chr < Symbols::SPACE.first() || chr == Symbols::DEL.first()) {
                    out += "\\x";
                    const auto byteVal = static_cast<std::byte>(chr);
                    const auto hiNibble = std::to_integer<unsigned int>(byteVal >> 4);
                    const auto loNibble = std::to_integer<unsigned int>(byteVal & std::byte{0x0F});
                    const auto toHex = [](unsigned int n) -> char {
                        constexpr unsigned int HexDigitOffset = 10U;
                        return static_cast<char>(n < HexDigitOffset ? ('0' + static_cast<int>(n))
                                                                      : ('A' + static_cast<int>(n - HexDigitOffset)));
                    };
                    out.push_back(toHex(hiNibble));
                    out.push_back(toHex(loNibble));
                } else {
                    out.push_back(static_cast<char>(chr));
                }
        }
    }
    return out;
}

} // namespace gwbasic
