// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/DetectDefaultTriple.h"
#include "basic_compiler/Symbols.h"

#include <array>
#include <cstddef>
#include <cstdio>
#include <stdio.h>
#include <string>

/*
 * Function: detectDefaultTriple
 * Inputs:
 *  - clangPath: Path to clang executable to query
 * Outputs:
 *  - std::string: Effective IR -triple parsed from clang's -### output
 * Theory of operation:
 *  - Spawns 'clang -### -S -x ir - -o /dev/null' and scans its stderr for a
 *    "-triple" argument, returning the value when found.
 */
// NOLINTBEGIN(readability-function-size)
std::string detectDefaultTriple(const std::string& clangPath) {
    std::string triple;
    const std::string cmd = clangPath + " -### -S -x ir - -o /dev/null 2>&1";
    if (FILE *pipe = popen(cmd.c_str(), "r")) { // NOLINT(misc-include-cleaner)
        constexpr std::size_t kBufSize = 256;
        std::array<char, kBufSize> buf{};
        std::string out;
        while (const std::size_t readCount = fread(buf.data(), 1, buf.size(), pipe)) { out.append(buf.data(), readCount); }
        pclose(pipe); // NOLINT(misc-include-cleaner)
        if (const auto pos = out.find("\"-triple\""); pos != std::string::npos) {
            if (const auto firstQuote = out.find(gwbasic::Symbols::DOUBLE_QUOTE.first(), pos + 9); firstQuote != std::string::npos) {
                if (const auto secondQuote = out.find(gwbasic::Symbols::DOUBLE_QUOTE.first(), firstQuote + 1); secondQuote != std::string::npos && secondQuote > firstQuote + 1) {
                    triple = out.substr(firstQuote + 1, secondQuote - (firstQuote + 1));
                }
            }
        }
    }
    return triple;
}
// NOLINTEND(readability-function-size)
