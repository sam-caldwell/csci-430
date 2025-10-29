// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>

/**
 * Function: asmCommentLeaderForTriple
 * Inputs:
 *  - triple: LLVM target triple (e.g., x86_64-linux-gnu, aarch64-apple-macos)
 * Outputs:
 *  - std::string: Line-comment prefix suitable for the target assembler
 * Theory of operation:
 *  - For supported targets, map the triple to a safe line comment token:
 *    - AArch64/ARM64 → "//" (GAS AArch64 uses //; # is immediate)
 *    - Default (x86_64 GNU/ELF, Mach-O) → "#"
 * ToDo: I'm too damned tired.  review this...was it necessary, did it work?
 */
std::string asmCommentLeaderForTriple(const std::string& triple);
