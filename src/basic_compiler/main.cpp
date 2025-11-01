// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <fstream>
#include <iostream>
#include <string>
#include <optional>
#include <filesystem>
#include <cstdlib>
#include <cctype>
#include <sstream>

#include "basic_compiler/Compiler.h"
#include "basic_compiler/AsmUtils.h"
#include "basic_compiler/TargetUtils.h"
#include "basic_compiler/Usage.h"
#include "basic_compiler/cli/TakeOptValue.h"
#include "basic_compiler/cli/TakeOptValues.h"

/**
 * Function: main
 * Inputs:
 *  - argc/argv: See 'usage' for flags.
 * Outputs:
 *  - int: Exit status (0=success)
 * Theory of operation:
 *  - Parses CLI flags, compiles the input BASIC file through the compiler
 *    pipeline with optional phase logs, and optionally materializes IR,
 *    bitcode, assembly, or a linked executable using the configured clang.
 */
int main(int argc, char** argv) {
    using gwbasic::cli::takeOptValue; // bring CLI helpers into scope
    if (argc < 2) { usage(argv[0]); return 2; }

    // Handle help in first position
    if (std::string first = argv[1]; first == "-h" || first == "--help") {
        usage(argv[0]); return 0;
    }

    std::string input = argv[1];
    std::optional<std::string> outLL;
    std::optional<std::string> outBC;
    std::optional<std::string> outBIN;
    std::optional<std::string> outASM;
    std::optional<std::string> targetTriple;
    std::optional<std::string> logPath;
    std::optional<std::string> lexLogPath;
    std::optional<std::string> syntaxLogPath;
    std::optional<std::string> semanticLogPath;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];

        // Outputs
        if (takeOptValue(a, "--bc", i, argc, argv, outBC)) continue;     // LLVM bitcode (.bc)
        if (takeOptValue(a, {"-ll", "--ll"}, i, argc, argv, outLL)) continue;   // LLVM IR text (.ll)
        if (takeOptValue(a, "-o", i, argc, argv, outBIN)) continue;      // Native binary (...overkill)
        if (takeOptValue(a, "--asm", i, argc, argv, outASM)) continue;   // Assembly (.asm: arm64? amd64?)

        // Target triple + logs
        if (takeOptValue(a, "--target", i, argc, argv, targetTriple)) continue;
        // ToDo: use Preprocessor directive to exclude log flags
        //       ...need corresponding flags for the logging functionality.
        if (takeOptValue(a, "--log", i, argc, argv, logPath)) continue;
        if (takeOptValue(a, "--lex-log", i, argc, argv, lexLogPath)) continue;
        if (takeOptValue(a, "--syntax-log", i, argc, argv, syntaxLogPath)) continue;
        if (takeOptValue(a, "--semantic-log", i, argc, argv, semanticLogPath)) continue;

        // DO NOT ADD MORE...YOU ARE OVERCOMPLICATING THINGS!

        std::cerr << "Unknown argument: " << a << "\n";
        usage(argv[0]);
        return 2;
    }
    if (targetTriple && !isSupportedTargetTriple(*targetTriple)) {
        std::cerr << "Error: unsupported target triple: " << *targetTriple
                  << " (supported: x86_64 or arm64/aarch64 on Linux/macOS/FreeBSD/Android)\n";
        return 2;
    }
    try {
        if (!logPath) {
            std::filesystem::path p = input;
            p.replace_extension(".codegen.log");
            logPath = p.string();
        }
        if (!lexLogPath) {
            std::filesystem::path p = input; p.replace_extension(".lex.log"); lexLogPath = p.string();
        }
        if (!syntaxLogPath) {
            std::filesystem::path p = input; p.replace_extension(".syntax.log"); syntaxLogPath = p.string();
        }
        if (!semanticLogPath) {
            std::filesystem::path p = input; p.replace_extension(".semantic.log"); semanticLogPath = p.string();
        }

        std::string ir = gwbasic::Compiler::compileFileWithPhaseLogs(
            input,
            *lexLogPath,
            *syntaxLogPath,
            *semanticLogPath,
            *logPath);

        if (outLL) {
            std::ofstream out(*outLL);
            out << ir;
        }
        if (outBC) {
#ifdef CLANG_PATH
            std::filesystem::path llTmp;
            if (outLL) {
                llTmp = *outLL;
            } else {
                llTmp = std::filesystem::path(*outBC).replace_extension(".ll");
                std::ofstream out(llTmp);
                out << ir;
            }
            std::ostringstream oss;
            oss << CLANG_PATH << " -c -emit-llvm -x ir \"" << llTmp.string() << "\" -o \"" << *outBC << "\"";
            std::string cmd = oss.str();
            int ec = std::system(cmd.c_str());
            if (ec != 0) {
                std::cerr << "clang failed assembling bitcode: " << cmd << "\n";
                return 1;
            }
#else
            std::cerr << "CLANG_PATH not defined at build time; cannot emit bitcode" << "\n";
            return 1;
#endif
        }
        if (outBIN) {
#ifdef CLANG_PATH
            std::filesystem::path llTmp;
            if (outLL) {
                llTmp = *outLL;
            } else {
                llTmp = std::filesystem::path(*outBIN).replace_extension(".ll");
                std::ofstream out(llTmp);
                out << ir;
            }
            std::ostringstream oss;
            oss << CLANG_PATH << ' ';
            if (targetTriple) oss << "-target \"" << *targetTriple << "\" ";
            oss << '"' << llTmp.string() << "\" -o \"" << *outBIN << "\"";
            // Link math library where required
            #if defined(__APPLE__)
            // libSystem provides libm; no extra flag needed
            #else
            oss << " -lm";
            #endif
            std::string cmd = oss.str();
            int ec = std::system(cmd.c_str());
            if (ec != 0) {
                std::cerr << "clang failed linking executable: " << cmd << "\n";
                return 1;
            }
#else
            std::cerr << "CLANG_PATH not defined at build time; cannot emit executable" << "\n";
            return 1;
#endif
        }
        if (outASM) {
#ifdef CLANG_PATH
            std::filesystem::path llTmp;
            if (outLL) {
                llTmp = *outLL;
            } else {
                std::filesystem::path asmOut = *outASM;
                if (asmOut.extension() != ".asm") asmOut += ".asm";
                // reflect enforced name back to outASM for consistency
                outASM = asmOut.string();
                llTmp = asmOut;
                llTmp.replace_extension(".ll");
                std::ofstream out(llTmp);
                out << ir;
            }
            std::string triple = targetTriple.value_or(std::string("arm64-apple-macos"));
            if (!isSupportedTargetTriple(triple)) {
                std::cerr << "Error: unsupported target triple for assembly: " << triple
                          << " (supported: x86_64 or arm64/aarch64 on Linux/macOS/FreeBSD/Android)\n";
                return 2;
            }
            std::ostringstream oss;
            oss << CLANG_PATH << " -S -x ir -target " << triple << " \"" << llTmp.string() << "\" -o \"" << *outASM << "\"";
            std::string cmd = oss.str();
            int ec = std::system(cmd.c_str());
            if (ec != 0) {
                std::cerr << "clang failed generating assembly: " << cmd << "\n";
                return 1;
            }
            // Prepend header comment with source file and target info
            try {
                auto srcName = std::filesystem::path(input).filename().string();
                std::string os = "unknown";
                std::string arch = triple;
                auto dash = triple.find('-');
                if (dash != std::string::npos) arch = triple.substr(0, dash);
                std::string lowerTriple = triple; for (auto& c : lowerTriple) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                if (lowerTriple.find("linux") != std::string::npos) os = "linux";
                else if (lowerTriple.find("macos") != std::string::npos || lowerTriple.find("darwin") != std::string::npos || lowerTriple.find("apple") != std::string::npos) os = "macos";
                else if (lowerTriple.find("freebsd") != std::string::npos) os = "freebsd";
                else if (lowerTriple.find("android") != std::string::npos) os = "android";
                // Choose a comment leader appropriate to the assembler dialect
                std::string commentLeader = asmCommentLeaderForTriple(triple);
                std::ifstream inAsm(*outASM);
                std::string body((std::istreambuf_iterator<char>(inAsm)), std::istreambuf_iterator<char>());
                inAsm.close();
                std::ofstream outAsm(*outASM, std::ios::trunc);
                outAsm << commentLeader << " Source: " << srcName
                       << " | Target: os=" << os << ", cpu=" << arch
                       << " (triple=" << triple << ")\n";
                outAsm << body;
            } catch (const std::exception& ex) {
                std::cerr << "warning: failed to prepend ASM header: " << ex.what() << "\n";
            }
#else
            std::cerr << "CLANG_PATH not defined at build time; cannot emit assembly" << "\n";
            return 1;
#endif
        }
        if (!outLL && !outBC && !outBIN && !outASM) {
            std::cout << ir;
        }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
