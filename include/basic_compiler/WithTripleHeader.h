// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

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
std::string withTripleHeader(const std::string& ir, const std::string& triple);

