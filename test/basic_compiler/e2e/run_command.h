// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <cstdio>
#include <string>

namespace e2e_helpers {

/**
 * Function: runCommand
 * Purpose:
 *  - Execute a shell command and capture its stdout as a string.
 * Inputs:
 *  - cmd: Full shell command to execute (will be passed to popen).
 * Outputs:
 *  - std::string: Captured stdout. Returns empty string on failure.
 */
inline std::string runCommand(const std::string& cmd) {
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return result;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) { result += buffer; }
    pclose(pipe);
    return result;
}

} // namespace e2e_helpers

