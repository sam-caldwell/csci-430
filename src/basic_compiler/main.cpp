// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <fstream>
#include <iostream>
#include <string>
#include <optional>
#include <filesystem>
#include <thread>
#include <chrono>

#include "basic_compiler/compiler/Compiler.h"
#include "basic_compiler/TargetUtils.h"
#include "basic_compiler/DetectDefaultTriple.h"
#include "basic_compiler/WithTripleHeader.h"
#include "basic_compiler/DeriveDefaultLogPaths.h"
#include "basic_compiler/WriteTextFile.h"
#include "basic_compiler/AssembleBitcode.h"
#include "../../include/basic_compiler/compiler/LinkBinary.h"
#include "../../include/basic_compiler/compiler/EmitAssembly.h"
#include "../../include/basic_compiler/cli/Usage.h"
#include "basic_compiler/cli/TakeOptValue.h"
#include "basic_compiler/cli/TakeOptValues.h"
#include "basic_compiler/compiler/Metrics.h"

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

    // Global watchdog: hard timeout to avoid runaway compile loops
    // If the process is still alive after TIMEOUT_S, force-exit.
    constexpr int TIMEOUT_S = 200;
    std::thread([=]{
        std::this_thread::sleep_for(std::chrono::seconds(TIMEOUT_S));
        std::fprintf(stderr, "Error: compiler timed out after %ds\n", TIMEOUT_S);
        std::fflush(stderr);
        std::_Exit(124);
    }).detach();

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
    bool wantMetrics = false;
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
        if (a == "--metrics") {
            wantMetrics = true;
            continue;
        }

        // Target triple + logs
        if (takeOptValue(a, "--target", i, argc, argv, targetTriple)) continue;
        // Debug helper: print effective IR triple used by clang and exit
        if (a == "--print-triple") {
            std::string t = detectDefaultTriple(CLANG_PATH);
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
        // Optional metrics context
        gwbasic::Metrics metrics;
        // ReSharper disable once CppDFALocalValueEscapesFunction
        if (wantMetrics) gwbasic::gMetrics = &metrics;
        deriveDefaultLogPaths(input, noLogs, logPath, lexLogPath, syntaxLogPath, semanticLogPath);
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
        const std::string chosenTriple = targetTriple.value_or(detectDefaultTriple(CLANG_PATH));
        const std::string irWithTriple = withTripleHeader(ir, chosenTriple);

        if (outLL) {
            writeTextFile(*outLL, irWithTriple);
        }
        if (outBC) {
            std::filesystem::path llTmp = outLL ? std::filesystem::path(*outLL)
                                                : std::filesystem::path(*outBC).replace_extension(".ll");
            if (!outLL) writeTextFile(llTmp, irWithTriple);
            if (int ec = assembleBitcode(llTmp, *outBC, CLANG_PATH); ec != 0) {
                std::cerr << "clang failed assembling bitcode\n";
                return 1;
            }
        }
        if (outBIN) {
            std::filesystem::path llTmp = outLL ? std::filesystem::path(*outLL)
                                                : std::filesystem::path(*outBIN).replace_extension(".ll");
            if (!outLL) writeTextFile(llTmp, irWithTriple);
            if (int ec = linkBinary(llTmp, *outBIN, chosenTriple, CLANG_PATH); ec != 0) {
                std::cerr << "clang failed linking executable\n";
                return 1;
            }
        }
        if (outASM) {
            std::filesystem::path asmOut = *outASM;
            if (asmOut.extension() != ".asm") asmOut += ".asm";
            // reflect enforced name back to outASM for consistency
            outASM = asmOut.string();
            std::filesystem::path llTmp = outLL ? std::filesystem::path(*outLL)
                                                : std::filesystem::path(*outASM).replace_extension(".ll");
            if (!outLL) writeTextFile(llTmp, irWithTriple);
            const std::string triple = targetTriple.value_or(detectDefaultTriple(CLANG_PATH));
            if (int ec = emitAssembly(llTmp, std::filesystem::path(*outASM), triple, std::filesystem::path(input), CLANG_PATH); ec != 0) {
                std::cerr << "clang failed generating assembly\n";
                return 1;
            }
        }
        if (!outLL && !outBC && !outBIN && !outASM) {
            std::cout << irWithTriple;
        }
        if (wantMetrics) {
            metrics.setOptPhaseCounts(gwbasic::Metrics::optimizedIrInstructionCounts(irWithTriple, CLANG_PATH, {"-O1", "-O2", "-O3"}));
            metrics.print();
            gwbasic::gMetrics = nullptr; // clear
        }
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
