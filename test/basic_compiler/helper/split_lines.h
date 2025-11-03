// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include <sstream>

/* Helper: splitLines
 * Purpose: Split a multi-line string into non-empty lines.
 * Components Under Test: Test utility only.
 * Expected Behavior: Returns vector of lines; empty lines are skipped.
 */
inline std::vector<std::string> splitLines(const std::string& s) {
    std::vector<std::string> out;
    std::istringstream iss(s);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty()) out.push_back(line);
    }
    return out;
}

