// NOLINTBEGIN(llvm-header-guard)
#ifndef BASIC_COMPILER_WITHTRIPLEHEADER_H
#define BASIC_COMPILER_WITHTRIPLEHEADER_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <string>

/**
 * Function: withTripleHeader
 * Purpose:
 *  - Ensure the given IR text declares a target triple.
 * Inputs:
 *  - ir: LLVM IR text to inspect.
 *  - triple: Target triple to inject if missing.
 * Outputs:
 *  - std::string: IR with a leading target triple if it was absent.
 */
std::string withTripleHeader(const std::string& irText, const std::string& triple);

#endif // BASIC_COMPILER_WITHTRIPLEHEADER_H
// NOLINTEND(llvm-header-guard)
