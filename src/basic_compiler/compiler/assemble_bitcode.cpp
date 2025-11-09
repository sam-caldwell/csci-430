// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/AssembleBitcode.h"
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>

/*
 * Function: assembleBitcode
 * Inputs:
 *  - llTmp: Path to the input LLVM IR (.ll) file
 *  - outBC: Path to the output LLVM bitcode (.bc) file
 *  - clangPath: Path to clang executable
 * Outputs:
 *  - int: Exit code from clang (0 indicates success)
 * Theory of operation:
 *  - Invokes clang with '-c -emit-llvm -x ir' to assemble IR into bitcode.
 */
int assembleBitcode(const std::filesystem::path &llTmp, const std::string &outBC, const std::string& clangPath) {
    std::ostringstream oss;
    oss << clangPath << " -c -emit-llvm -x ir \"" << llTmp.string() << "\" -o \"" << outBC << "\"";
    return std::system(oss.str().c_str()); // NOLINT(concurrency-mt-unsafe)
}
