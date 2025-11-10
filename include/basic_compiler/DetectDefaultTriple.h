// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_DETECTDEFAULTTRIPLE_H
#define BASIC_COMPILER_DETECTDEFAULTTRIPLE_H

#include <string>

/**
 * Function: detectDefaultTriple
 * Purpose:
 *  - Query clang for its effective IR compilation target triple.
 * Inputs:
 *  - clangPath: Path to the clang executable to query.
 * Outputs:
 *  - std::string: Parsed target triple (e.g., x86_64-linux-gnu), or empty on failure.
 */
std::string detectDefaultTriple(const std::string& clangPath);

#endif // BASIC_COMPILER_DETECTDEFAULTTRIPLE_H
