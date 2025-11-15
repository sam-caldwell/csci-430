// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <string>

namespace gwbasic {

/*
 * Function: appendEscapedByte
 * Summary: Append a byte to an IR string literal, escaping as needed.
 * Parameters:
 *  - out: Target string to append to.
 *  - c: Byte to encode.
 * Returns:
 *  - void
 */
void CodeGenerator::appendEscapedByte(std::string& out, unsigned char ch_byte) {
    // Named constants to avoid magic numbers and clarify ranges
    constexpr unsigned char kFirstPrintable = 32;   // ' '
    constexpr unsigned char kDel            = 127;  // DEL
    constexpr unsigned char kNibbleMask     = 0x0F;
    constexpr unsigned int  kHighNibbleBits = 4U;
    constexpr unsigned char kHexAStart      = 10U;

    if (ch_byte >= kFirstPrintable && ch_byte < kDel) {
        out.push_back(static_cast<char>(ch_byte));
        return;
    }
    if (ch_byte == static_cast<unsigned char>('\\')) { out += "\\5C"; return; }
    if (ch_byte == static_cast<unsigned char>(Symbols::DOUBLE_QUOTE.first())) { out += "\\22"; return; }
    if (ch_byte == static_cast<unsigned char>(Symbols::LF.first())) { out += "\\0A"; return; }
    if (ch_byte == static_cast<unsigned char>('\t')) { out += "\\09"; return; }
    if (ch_byte == static_cast<unsigned char>('\r')) { out += "\\0D"; return; }

    // Hex-encode as "\\XY" using uppercase hex digits without varargs
    out.push_back('\\');
    const unsigned char hiNibble = static_cast<unsigned char>((ch_byte >> kHighNibbleBits) & kNibbleMask);
    const unsigned char loNibble = static_cast<unsigned char>(ch_byte & kNibbleMask);
    const char hiCh = (hiNibble < kHexAStart)
                      ? static_cast<char>('0' + hiNibble)
                      : static_cast<char>('A' + (hiNibble - kHexAStart));
    const char loCh = (loNibble < kHexAStart)
                      ? static_cast<char>('0' + loNibble)
                      : static_cast<char>('A' + (loNibble - kHexAStart));
    out.push_back(hiCh);
    out.push_back(loCh);
}

} // namespace gwbasic
