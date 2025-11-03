// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <filesystem>
#include <string>

/**
 * Function: assembleBitcode
 * Purpose:
 *  - Assemble LLVM IR (.ll) into LLVM bitcode (.bc) using clang.
 * Inputs:
 *  - llTmp: Path to input .ll file.
 *  - outBC: Path to output .bc file.
 *  - clangPath: Path to clang executable.
 * Outputs:
 *  - int: Process exit code (0 on success).
 */
int assembleBitcode(const std::filesystem::path &llTmp, const std::string &outBC, const std::string& clangPath);

