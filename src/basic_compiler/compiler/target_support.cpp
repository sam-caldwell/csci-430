// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/TargetUtils.h"

#include <cctype>
#include <string>

/*
 * Function: isSupportedTargetTriple
 * Inputs:
 *  - triple: LLVM target triple string
 * Outputs:
 *  - bool: true if arch is x86_64 or arm64 and OS is Linux or macOS
 * Theory of operation:
 *  - Lowercases the input and checks for expected substrings to determine
 *    compatibility with the toolchain.
 */
bool isSupportedTargetTriple(const std::string& triple) {
    std::string lowerTriple = triple;
    for (auto& chr : lowerTriple) { chr = static_cast<char>(std::tolower(static_cast<unsigned char>(chr))); }
    // Arch: x86_64 or arm64/aarch64 only
    const bool archOK = (lowerTriple.find("x86_64") != std::string::npos) ||
                        (lowerTriple.find("arm64") != std::string::npos) ||
                        (lowerTriple.find("aarch64") != std::string::npos);
    // OS: linux, darwin, macos/macosx
    const bool osOK = (lowerTriple.find("linux") != std::string::npos) ||
                      (lowerTriple.find("darwin") != std::string::npos) ||
                      (lowerTriple.find("macos") != std::string::npos);
    return archOK && osOK;
}
