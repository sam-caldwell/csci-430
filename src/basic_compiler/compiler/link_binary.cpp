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
    // On macOS when using Homebrew clang, ensure the SDK sysroot is provided so
    // libSystem and related system libs are discoverable.
    std::string lowerTriple = triple;
    for (auto &c : lowerTriple) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    const bool targetingDarwin = (!lowerTriple.empty()) &&
                                 (lowerTriple.find("darwin") != std::string::npos ||
                                  lowerTriple.find("macos") != std::string::npos ||
                                  lowerTriple.find("macosx") != std::string::npos);
    if (targetingDarwin) {
        // Defer SDK path resolution to xcrun at runtime (works on GitHub macOS runners).
        oss << "-isysroot $(xcrun --sdk macosx --show-sdk-path) ";
    }
    oss << '"' << llTmp.string() << "\" -o \"" << outBIN << "\"";
#if defined(__APPLE__)
    // libSystem provides libm; no extra flag needed
#else
    oss << " -lm";
#endif
    return std::system(oss.str().c_str());
}
