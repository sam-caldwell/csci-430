// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::appendEscapedByte
 * Purpose:
 *  - Append a single byte to an IR string literal, escaping as needed.
 * Inputs:
 *  - out: target string to append to
 *  - c: byte to encode
 */
void CodeGenerator::appendEscapedByte(std::string& out, unsigned char c) {
    // Named constants to avoid magic numbers and clarify ranges
    constexpr unsigned char kFirstPrintable = 32;   // ' '
    constexpr unsigned char kDel            = 127;  // DEL
    constexpr unsigned char kNibbleMask     = 0x0F;
    constexpr unsigned int  kHighNibbleBits = 4U;
    constexpr unsigned char kHexAStart      = 10U;

    if (c >= kFirstPrintable && c < kDel) {
        out.push_back(static_cast<char>(c));
        return;
    }
    if (c == static_cast<unsigned char>('\\')) { out += "\\5C"; return; }
    if (c == static_cast<unsigned char>(Symbols::DOUBLE_QUOTE.first())) { out += "\\22"; return; }
    if (c == static_cast<unsigned char>(Symbols::LF.first())) { out += "\\0A"; return; }
    if (c == static_cast<unsigned char>('\t')) { out += "\\09"; return; }
    if (c == static_cast<unsigned char>('\r')) { out += "\\0D"; return; }

    // Hex-encode as "\\XY" using uppercase hex digits without varargs
    out.push_back('\\');
    const unsigned char hi = static_cast<unsigned char>((c >> kHighNibbleBits) & kNibbleMask);
    const unsigned char lo = static_cast<unsigned char>(c & kNibbleMask);
    const char hiCh = (hi < kHexAStart)
                      ? static_cast<char>('0' + hi)
                      : static_cast<char>('A' + (hi - kHexAStart));
    const char loCh = (lo < kHexAStart)
                      ? static_cast<char>('0' + lo)
                      : static_cast<char>('A' + (lo - kHexAStart));
    out.push_back(hiCh);
    out.push_back(loCh);
}

} // namespace gwbasic

