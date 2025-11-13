// NOLINTBEGIN(llvm-header-guard)
#ifndef BASIC_COMPILER_ASSEMBLEBITCODE_H
#define BASIC_COMPILER_ASSEMBLEBITCODE_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

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

#endif // BASIC_COMPILER_ASSEMBLEBITCODE_H
// NOLINTEND(llvm-header-guard)
