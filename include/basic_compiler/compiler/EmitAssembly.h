// NOLINTBEGIN(llvm-header-guard)
#ifndef BASIC_COMPILER_EMITASSEMBLY_H
#define BASIC_COMPILER_EMITASSEMBLY_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <filesystem>
#include <string>
#include <string_view>

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
int emitAssembly(const std::filesystem::path &llTmp,
                 const std::filesystem::path &asmOut,
                 std::string_view triple,
                 const std::filesystem::path &input,
                 std::string_view clangPath);

#endif // BASIC_COMPILER_EMITASSEMBLY_H
// NOLINTEND(llvm-header-guard)
