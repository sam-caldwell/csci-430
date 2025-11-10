// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_TARGETUTILS_H
#define BASIC_COMPILER_TARGETUTILS_H

#include <string>

/**
 * Function: isSupportedTargetTriple
 * Purpose:
 *  - Validate that the target triple is within the supported set.
 * Inputs:
 *  - triple: LLVM target triple string
 * Outputs:
 *  - bool: true if the triple is supported by this toolchain
 */
bool isSupportedTargetTriple(const std::string& triple);

#endif // BASIC_COMPILER_TARGETUTILS_H
