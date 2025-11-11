// (c) 2025 Sam Caldwell. All Rights Reserved.
// Purpose: Library to validate one-function-per-file convention for C/C++ sources.

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace onefunc {

struct Issue {
    std::string file;
    int line;               // 1-based (location of extra function)
    std::string message;
};

class OneFuncChecker {
public:
    void addPath(const std::filesystem::path& path);
    std::vector<Issue> run();
    static std::vector<Issue> checkContent(const std::string& content,
                                           const std::filesystem::path& path);

private:
    std::vector<std::filesystem::path> paths_;
    static bool isSourceFile(const std::filesystem::path& path);
};

} // namespace onefunc
