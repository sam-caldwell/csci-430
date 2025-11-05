// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/EmitAssembly.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/AsmUtils.h"
#include "basic_compiler/TargetUtils.h"
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>

/*
 * Function: emitAssembly
 * Inputs:
 *  - llTmp: Path to the input LLVM IR (.ll) file
 *  - asmOut: Path to the output assembly (.asm) file
 *  - triple: Target triple to emit for (must be supported)
 *  - input: Original source path (for header comment metadata)
 *  - clangPath: Path to clang executable
 * Outputs:
 *  - int: Exit code from clang (0 indicates success)
 * Theory of operation:
 *  - Calls clang to generate assembly and then prepends a header comment with
 *    source and target information tailored to the assembler dialect.
 */
int emitAssembly(const std::filesystem::path &llTmp,
                 const std::string &asmOut,
                 const std::string &triple,
                 const std::string &input,
                 const std::string& clangPath) {
    if (!isSupportedTargetTriple(triple)) {
        std::cerr << "Error: unsupported target triple for assembly: " << triple
                  << " (supported: x86_64 or arm64/aarch64 on Linux/macOS)\n";
        return 2;
    }
    std::ostringstream oss;
    oss << clangPath << " -S -x ir -target " << triple << " \"" << llTmp.string() << "\" -o \"" << asmOut << "\"";
    int ec = std::system(oss.str().c_str());
    if (ec != 0) return ec;

    try {
        auto srcName = std::filesystem::path(input).filename().string();
        std::string os = "unknown";
        std::string arch = triple;
        if (auto dash = triple.find(gwbasic::Symbols::MINUS.first()); dash != std::string::npos) arch = triple.substr(0, dash);
        std::string lowerTriple = triple;
        for (auto &c: lowerTriple) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (lowerTriple.find("linux") != std::string::npos) os = "linux";
        else if (lowerTriple.find("macos") != std::string::npos || lowerTriple.find("darwin") != std::string::npos)
            os = "macos";
        std::string commentLeader = asmCommentLeaderForTriple(triple);
        std::ifstream inAsm(asmOut);
        std::string body((std::istreambuf_iterator<char>(inAsm)), std::istreambuf_iterator<char>());
        inAsm.close();
        std::ofstream outAsm(asmOut, std::ios::trunc);
        outAsm << commentLeader << " Source: " << srcName
               << " | Target: os=" << os << ", cpu=" << arch
               << " (triple=" << triple << ")\n";
        outAsm << body;
    } catch (const std::exception &ex) {
        std::cerr << "warning: failed to prepend ASM header: " << ex.what() << "\n";
    }
    return 0;
}
