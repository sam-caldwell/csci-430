// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>

/**
 * Function: isSupportedTargetTriple
 * Inputs:
 *  - triple: LLVM target triple string
 * Outputs:
 *  - bool: true if the triple is supported by this toolchain
 * Theory of operation:
 *  - Allows only: x86_64 and arm64/aarch64 architectures on Linux, macOS (Darwin/Apple),
 *    FreeBSD, or Android; rejects Windows/MSVC and WebAssembly.
 */
bool isSupportedTargetTriple(const std::string& triple);

