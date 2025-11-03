// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/LinkBinary.h"
#include <sstream>

/*
 * Function: linkBinary
 * Inputs:
 *  - llTmp: Path to the input LLVM IR (.ll) file
 *  - outBIN: Path to the output executable
 *  - triple: Target triple to link for (may be empty for host default)
 *  - clangPath: Path to clang executable
 * Outputs:
 *  - int: Exit code from clang (0 indicates success)
 * Theory of operation:
 *  - Invokes clang to link the IR, adding ' -lm' on non-Apple platforms.
 */
int linkBinary(const std::filesystem::path &llTmp, const std::string &outBIN, const std::string &triple, const std::string& clangPath) {
    std::ostringstream oss;
    oss << clangPath << ' ';
    if (!triple.empty()) oss << "-target \"" << triple << "\" ";
    oss << '"' << llTmp.string() << "\" -o \"" << outBIN << "\"";
#if defined(__APPLE__)
    // libSystem provides libm; no extra flag needed
#else
    oss << " -lm";
#endif
    return std::system(oss.str().c_str());
}
