// (c) 2025 Sam Caldwell. All Rights Reserved.
// CLI for clang-tidy-docstring checker

#include "clang-tidy-docstring/DocstringChecker.h"

#include <iostream>
#include <string>
#include <vector>

static void printUsage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " [-d <dir|file>]...\n";
    std::cerr << "Scans given dirs/files (recursively) for function/method definitions\n"
              << "and validates that a docstring immediately precedes each definition\n"
              << "with fields: Function:, Parameters:, Returns: (or Outputs:).\n";
}

int main(const int argc, char** argv) {
    using namespace doccheck;
    DocstringChecker checker;
    std::vector<std::string> inputs;

    // Default: scan ./include and ./src if no -d given
    if (argc == 1) {
        inputs.emplace_back("include");
        inputs.emplace_back("src");
    } else {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") { printUsage(argv[0]); return 0; }
            if (arg == "-d" && i + 1 < argc) {
                inputs.emplace_back(argv[++i]);
            } else {
                std::cerr << "Unknown argument: " << arg << "\n";
                printUsage(argv[0]);
                return 2;
            }
        }
        if (inputs.empty()) { inputs = {"include", "src"}; }
    }

    for (const auto &p : inputs) checker.addPath(p);
    auto issues = checker.run();
    if (issues.empty()) {
        std::cout << "Docstring check: OK\n";
        return 0;
    }
    for (const auto &is : issues) {
        std::cerr << is.file << ":" << is.line << ": error: " << is.message
                  << " [clang-tidy-docstring]" << "\n";
    }
    std::cerr << "Docstring check failed with " << issues.size() << " issue(s).\n";
    return 1;
}
