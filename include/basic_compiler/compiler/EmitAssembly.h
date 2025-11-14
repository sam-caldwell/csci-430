// NOLINTBEGIN(llvm-header-guard)
#ifndef BASIC_COMPILER_EMITASSEMBLY_H
#define BASIC_COMPILER_EMITASSEMBLY_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <filesystem>
#include <string>

/**
 * Function: emitAssembly
 * Purpose:
 *  - Emit assembly from LLVM IR (.ll) using clang and prepend a source/target header comment.
 * Inputs:
 *  - llTmp: Path to input .ll file.
 *  - asmOut: Path to output .asm file.
 *  - triple: Target triple string (must be supported by toolchain).
 *  - input: Original source path (for header comment metadata).
 *  - clangPath: Path to clang executable.
 * Outputs:
 *  - int: Process exit code (0 on success).
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
int emitAssembly(const std::filesystem::path &llTmp,
                 const std::string &asmOut,
                 const std::string &triple,
                 const std::string &input,
                 const std::string &clangPath);

#endif // BASIC_COMPILER_EMITASSEMBLY_H
// NOLINTEND(llvm-header-guard)
