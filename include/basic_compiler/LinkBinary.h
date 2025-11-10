// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_LINKBINARY_H
#define BASIC_COMPILER_LINKBINARY_H

#include <filesystem>
#include <string>

/**
 * Function: linkBinary
 * Purpose:
 *  - Link a native executable from LLVM IR (.ll) using clang.
 * Inputs:
 *  - llTmp: Path to input .ll file.
 *  - outBIN: Output binary path.
 *  - triple: Target triple string (may be empty for host default).
 *  - clangPath: Path to clang executable.
 * Outputs:
 *  - int: Process exit code (0 on success).
 */
int linkBinary(const std::filesystem::path &llTmp, const std::string &outBIN, const std::string &triple, const std::string& clangPath);

#endif // BASIC_COMPILER_LINKBINARY_H
