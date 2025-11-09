// (c) 2025 Sam Caldwell. All Rights Reserved.
// Purpose: Library to validate that each function/method definition
//          in C/C++ sources has a docstring describing name, parameters, and returns.

#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace doccheck {

struct Issue {
    std::string file;
    int line;               // 1-based
    std::string message;
};

class DocstringChecker {
public:
    // Add directories or files to check. Directories are scanned recursively.
    void addPath(const std::filesystem::path& p);

    // Run the checker over all added paths. Returns a list of issues.
    std::vector<Issue> run();

    // Exposed for unit tests: check raw content as if it came from 'path'.
    std::vector<Issue> checkContent(const std::string& content,
                                    const std::filesystem::path& path) const;

    // Utility: lowercase a string (exposed for helpers)
    static std::string toLower(std::string s);

private:
    std::vector<std::filesystem::path> paths_{};

    static bool isSourceFile(const std::filesystem::path& p);
};

} // namespace doccheck
