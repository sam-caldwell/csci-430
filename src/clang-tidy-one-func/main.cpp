// (c) 2025 Sam Caldwell. All Rights Reserved.
// CLI for one-function-per-file checker

#include "clang-tidy-one-func/OneFuncChecker.h"

#include <exception>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

static void printUsage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " [-d <dir|file>]...\n";
    std::cerr << "Scans given dirs/files (recursively) and ensures each source file contains\n"
              << "at most one function/method definition.\n";
}

struct ShowHelp final : public std::exception { };
static std::string gProgramName = "onefunc";

static std::vector<std::string> gatherInputs(int argc, char** argv) {
    std::vector<std::string> inputs;
    std::span<char* const> args{argv, static_cast<std::size_t>(argc)};
    if (args.size() == 1) {
        inputs.emplace_back("src/basic_compiler");
        return inputs;
    }
    for (std::size_t i = 1; i < args.size(); ++i) {
        const std::string arg = args[i];
        if (arg == "-h" || arg == "--help") {
            throw ShowHelp{};
        }
        if (arg == "-d" && i + 1 < args.size()) {
            inputs.emplace_back(args[++i]);
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }
    if (inputs.empty()) {
        inputs.emplace_back("src/basic_compiler");
    }
    return inputs;
}

int main(int argc, char** argv) try {
    using namespace onefunc;
    OneFuncChecker checker;
    const std::vector<std::string> inputs = gatherInputs(argc, argv);
    {
        std::span<char* const> args{argv, static_cast<std::size_t>(argc)};
        if (!args.empty() && args.front() != nullptr) {
            gProgramName = args.front();
        }
    }
    for (const auto &path : inputs) {
        checker.addPath(path);
    }
    const auto issues = checker.run();
    if (issues.empty()) {
        std::cout << "One-function-per-file check: OK\n";
        return 0;
    }
    for (const auto &issue : issues) {
        std::cerr << issue.file << ":" << issue.line << ": error: " << issue.message
                  << " [clang-tidy-one-func]\n";
    }
    std::cerr << "One-function-per-file check failed with " << issues.size() << " issue(s).\n";
    return 1;
} catch (const ShowHelp&) {
    printUsage(gProgramName.c_str());
    return 0;
} catch (const std::exception& ex) {
    std::cerr << "error: " << ex.what() << "\n";
    return 2;
} catch (...) {
    std::cerr << "error: unknown exception\n";
    return 2;
}
