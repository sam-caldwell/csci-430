// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>

/* Helper: irBlockSlice
 * Purpose: Extract a labeled IR basic block from a text blob.
 * Components Under Test: Test utility only.
 * Expected Behavior: Returns substring from label to next "\nline" or end.
 */
inline std::string irBlockSlice(const std::string& ir, const std::string& label) {
    const auto pos = ir.find(label + ":\n");
    if (pos == std::string::npos) return std::string{};
    auto end = ir.find("\nline", pos + 1);
    if (end == std::string::npos) end = ir.size();
    return ir.substr(pos, end - pos);
}

