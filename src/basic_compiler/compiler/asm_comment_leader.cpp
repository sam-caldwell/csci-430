// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/AsmUtils.h"
#include <cctype>

/**
 * Function: asmCommentLeaderForTriple
 * Inputs:
 *  - triple: LLVM target triple (e.g., x86_64-linux-gnu, aarch64-apple-macos)
 * Outputs:
 *  - std::string: Line-comment prefix suitable for the target assembler
 * Theory of operation:
 *  - Map supported targets to their line-comment tokens.
 *    - AArch64/ARM64 (…aarch64…/…arm64…): "//" (GAS AArch64 uses //; # is immediate)
 *    - Default GNU/ELF/Mach-O (x86/x86_64, etc.): "#"
 */
std::string asmCommentLeaderForTriple(const std::string& triple) {
    std::string t = triple;
    for (auto& c : t) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if (t.find("aarch64") != std::string::npos || t.find("arm64") != std::string::npos) return "//";
    return "#";
}

