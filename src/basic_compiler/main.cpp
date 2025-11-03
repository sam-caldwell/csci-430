// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <fstream>
#include <iostream>
#include <string>
#include <optional>
#include <filesystem>
#include <sstream>

#include "basic_compiler/Compiler.h"
#include "basic_compiler/AsmUtils.h"
#include "basic_compiler/TargetUtils.h"
#include "basic_compiler/Usage.h"
#include "basic_compiler/cli/TakeOptValue.h"
#include "basic_compiler/cli/TakeOptValues.h"

#ifndef CLANG_PATH
# error "CLANG_PATH not defined at build time; cannot emit bitcode"
#endif

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
int main(int argc, char **argv) {

    using gwbasic::cli::takeOptValue; // bring CLI helpers into scope

    if (argc < 2) {
        usage(argv[0]);
        return 2;
    }

    // Handle help in first position
    if (std::string first = argv[1]; first == "-h" || first == "--help") {
        usage(argv[0]);
        return 0;
    }

    std::string input = argv[1];
    // Helper to detect clang's default target triple (for aligning IR)
    auto detectDefaultTriple = []() -> std::string {
        std::string triple;
        // Ask clang how it will invoke cc1 for IR, and parse the -triple it uses
        const std::string cmd = std::string(CLANG_PATH) + " -### -S -x ir - -o /dev/null 2>&1";
        if (FILE *pipe = popen(cmd.c_str(), "r")) {
            char buf[256];
            std::string out;
            while (const size_t n = fread(buf, 1, sizeof(buf), pipe)) out.append(buf, buf + n);
            pclose(pipe);
            // Find -triple "..."
            if (const auto pos = out.find("\"-triple\""); pos != std::string::npos) {
                if (const auto q1 = out.find('"', pos + 9); q1 != std::string::npos) {
                    if (const auto q2 = out.find('"', q1 + 1); q2 != std::string::npos && q2 > q1 + 1) {
                        triple = out.substr(q1 + 1, q2 - (q1 + 1));
                    }
                }
            }
        }
        return triple;
    };
    auto withTripleHeader = [](const std::string &ir, const std::string &triple) -> std::string {
        if (triple.empty()) return ir;
        if (ir.find("target triple =") != std::string::npos) return ir;
        std::ostringstream out;
        out << "target triple = \"" << triple << "\"\n\n" << ir;
        return out.str();
    };
    std::optional<std::string> outLL;
    std::optional<std::string> outBC;
    std::optional<std::string> outBIN;
    std::optional<std::string> outASM;
    std::optional<std::string> targetTriple;
    std::optional<std::string> logPath;
    std::optional<std::string> lexLogPath;
    std::optional<std::string> syntaxLogPath;
    std::optional<std::string> semanticLogPath;
    bool noLogs = false;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];

        // Outputs
        if (takeOptValue(a, "--bc", i, argc, argv, outBC)) continue; // LLVM bitcode (.bc)
        if (takeOptValue(a, {"-ll", "--ll"}, i, argc, argv, outLL)) continue; // LLVM IR text (.ll)
        if (takeOptValue(a, "-o", i, argc, argv, outBIN)) continue; // Native binary (...overkill)
        if (takeOptValue(a, "--asm", i, argc, argv, outASM)) continue; // Assembly (.asm: arm64? amd64?)

        // Custom flags
        if (a == "--no-logs") {
            noLogs = true;
            continue;
        }

        // Target triple + logs
        if (takeOptValue(a, "--target", i, argc, argv, targetTriple)) continue;
        // Debug helper: print effective IR triple used by clang and exit
        if (a == "--print-triple") {
            std::string t = detectDefaultTriple();
            std::cout << t << '\n';
            return 0;
        }
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
                << " (supported: x86_64 or arm64/aarch64 on Linux/macOS)\n";
        return 2;
    }
    try {
        if (!noLogs && !logPath) {
            std::filesystem::path p = input;
            p.replace_extension(".codegen.log");
            logPath = p.string();
        }
        if (!noLogs && !lexLogPath) {
            std::filesystem::path p = input;
            p.replace_extension(".lex.log");
            lexLogPath = p.string();
        }
        if (!noLogs && !syntaxLogPath) {
            std::filesystem::path p = input;
            p.replace_extension(".syntax.log");
            syntaxLogPath = p.string();
        }
        if (!noLogs && !semanticLogPath) {
            std::filesystem::path p = input;
            p.replace_extension(".semantic.log");
            semanticLogPath = p.string();
        }
        std::string ir;
        if (!noLogs) {
            ir = gwbasic::Compiler::compileFileWithPhaseLogs(
                input,
                *lexLogPath,
                *syntaxLogPath,
                *semanticLogPath,
                *logPath);
        } else {
            ir = gwbasic::Compiler::compileFile(input);
        }
        const std::string chosenTriple = targetTriple.value_or(detectDefaultTriple());
        const std::string irWithTriple = withTripleHeader(ir, chosenTriple);

        if (outLL) {
            std::ofstream out(*outLL);
            out << irWithTriple;
        }
        if (outBC) {
            std::filesystem::path llTmp;
            if (outLL) {
                llTmp = *outLL;
            } else {
                llTmp = std::filesystem::path(*outBC).replace_extension(".ll");
                std::ofstream out(llTmp);
                out << irWithTriple;
            }
            std::ostringstream oss;
            oss << CLANG_PATH << " -c -emit-llvm -x ir \"" << llTmp.string() << "\" -o \"" << *outBC << "\"";
            std::string cmd = oss.str();
            if (int ec = std::system(cmd.c_str()); ec != 0) {
                std::cerr << "clang failed assembling bitcode: " << cmd << "\n";
                return 1;
            }
        }
        if (outBIN) {
            std::filesystem::path llTmp;
            if (outLL) {
                llTmp = *outLL;
            } else {
                llTmp = std::filesystem::path(*outBIN).replace_extension(".ll");
                std::ofstream out(llTmp);
                out << irWithTriple;
            }
            std::ostringstream oss;
            oss << CLANG_PATH << ' ';
            if (!chosenTriple.empty()) oss << "-target \"" << chosenTriple << "\" ";
            oss << '"' << llTmp.string() << "\" -o \"" << *outBIN << "\"";
            // Link math library where required
#if defined(__APPLE__)
            // libSystem provides libm; no extra flag needed
#else
            oss << " -lm";
#endif
            std::string cmd = oss.str();
            if (int ec = std::system(cmd.c_str()); ec != 0) {
                std::cerr << "clang failed linking executable: " << cmd << "\n";
                return 1;
            }
        }
        if (outASM) {
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
                out << irWithTriple;
            }
            std::string triple = targetTriple.value_or(detectDefaultTriple());
            if (!isSupportedTargetTriple(triple)) {
                std::cerr << "Error: unsupported target triple for assembly: " << triple
                        << " (supported: x86_64 or arm64/aarch64 on Linux/macOS)\n";
                return 2;
            }
            std::ostringstream oss;
            oss << CLANG_PATH << " -S -x ir -target " << triple << " \"" << llTmp.string() << "\" -o \"" << *outASM <<
                    "\"";
            std::string cmd = oss.str();
            if (int ec = std::system(cmd.c_str()); ec != 0) {
                std::cerr << "clang failed generating assembly: " << cmd << "\n";
                return 1;
            }
            // Prepend header comment with source file and target info
            try {
                auto srcName = std::filesystem::path(input).filename().string();
                std::string os = "unknown";
                std::string arch = triple;
                if (auto dash = triple.find('-'); dash != std::string::npos)
                    arch = triple.substr(0, dash);
                std::string lowerTriple = triple;
                for (auto &c: lowerTriple) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                if (lowerTriple.find("linux") != std::string::npos) os = "linux";
                else if (lowerTriple.find("macos") != std::string::npos || lowerTriple.find("darwin") !=
                         std::string::npos) os = "macos";
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
            } catch (const std::exception &ex) {
                std::cerr << "warning: failed to prepend ASM header: " << ex.what() << "\n";
            }
        }
        if (!outLL && !outBC && !outBIN && !outASM) {
            std::cout << irWithTriple;
        }
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
