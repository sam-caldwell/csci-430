// (c) 2025 Sam Caldwell. All Rights Reserved.
// CLI for one-function-per-file checker

#include "clang-tidy-one-func/OneFuncChecker.h"

#include <iostream>
#include <string>
#include <vector>

static void printUsage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " [-d <dir|file>]...\n";
    std::cerr << "Scans given dirs/files (recursively) and ensures each source file contains\n"
              << "at most one function/method definition.\n";
}

int main(int argc, char** argv) {
    using namespace onefunc;
    OneFuncChecker checker;
    std::vector<std::string> inputs;

    if (argc == 1) {
        inputs.push_back("src/basic_compiler");
    } else {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") { printUsage(argv[0]); return 0; }
            if (arg == "-d" && i + 1 < argc) {
                inputs.push_back(argv[++i]);
            } else {
                std::cerr << "Unknown argument: " << arg << "\n";
                printUsage(argv[0]);
                return 2;
            }
        }
        if (inputs.empty()) { inputs = {"src/basic_compiler"}; }
    }

    for (const auto &p : inputs) checker.addPath(p);
    auto issues = checker.run();
    if (issues.empty()) {
        std::cout << "One-function-per-file check: OK\n";
        return 0;
    }
    for (const auto &is : issues) {
        std::cerr << is.file << ":" << is.line << ": error: " << is.message
                  << " [clang-tidy-one-func]\n";
    }
    std::cerr << "One-function-per-file check failed with " << issues.size() << " issue(s).\n";
    return 1;
}

