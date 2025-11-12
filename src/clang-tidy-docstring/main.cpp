// (c) 2025 Sam Caldwell. All Rights Reserved.
// CLI for clang-tidy-docstring checker

#include "clang-tidy-docstring/DocstringChecker.h"

#include <cstddef>
#include <exception>
#include <iostream>
#include <span>
#include <string>
#include <vector>

static void printUsage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " [-d <dir|file>]...\n";
    std::cerr << "Scans given dirs/files (recursively) for function/method definitions\n"
              << "and validates that a docstring immediately precedes each definition\n"
              << "with fields: Function:, Parameters:, Returns: (or Outputs:).\n";
}

int main(const int argc, char** argv) try { // NOLINT(readability-function-cognitive-complexity,readability-function-size)
    using namespace doccheck;
    DocstringChecker checker;
    std::vector<std::string> inputs;

    // Default: scan ./include and ./src if no -d given
    if (argc == 1) {
        inputs.emplace_back("include");
        inputs.emplace_back("src");
    } else {
        const std::span<char*> args(argv, static_cast<size_t>(argc));
        for (int i = 1; i < argc; ++i) {
            const std::string arg = args[static_cast<size_t>(i)];
            if (arg == "-h" || arg == "--help") {
                printUsage(args[0]);
                return 0;
            }
            if (arg == "-d" && i + 1 < argc) {
                inputs.emplace_back(args[static_cast<size_t>(++i)]);
            } else {
                std::cerr << "Unknown argument: " << arg << "\n";
                printUsage(args[0]);
                return 2;
            }
        }
        if (inputs.empty()) {
            inputs = {"include", "src"};
        }
    }

    for (const auto &inputPath : inputs) {
        checker.addPath(inputPath);
    }
    auto issues = checker.run();
    if (issues.empty()) {
        std::cout << "Docstring check: OK\n";
        return 0;
    }
    for (const auto &issue : issues) {
        std::cerr << issue.file << ":" << issue.line << ": error: " << issue.message
                  << " [clang-tidy-docstring]" << "\n";
    }
    std::cerr << "Docstring check failed with " << issues.size() << " issue(s).\n";
    return 1;
} catch (const std::exception& ex) {
    std::cerr << "fatal: unhandled exception: " << ex.what() << "\n";
    return 2;
} catch (...) {
    std::cerr << "fatal: unknown exception\n";
    return 2;
}
