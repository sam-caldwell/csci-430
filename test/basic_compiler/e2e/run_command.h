// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace e2e_helpers {

/**
 * Function: runCommand
 * Purpose:
 *  - Execute a shell command and capture its stdout as a string.
 *  - Apply a soft guard on captured output size to prevent OOM
 *    if a child process produces unbounded output.
 * Inputs:
 *  - cmd: Full shell command to execute (passed to popen).
 * Environment:
 *  - E2E_MAX_OUTPUT: optional integer (bytes) to cap captured stdout
 *                    (defaults to 131072 bytes).
 * Outputs:
 *  - std::string: Captured stdout (possibly truncated to the cap).
 *                  Returns empty string on failure.
 */
inline std::string runCommand(const std::string& cmd) {
    // Derive capture cap from environment (default 128 KiB)
    size_t max_bytes = 131072;
    if (const char* env = std::getenv("E2E_MAX_OUTPUT")) {
        try { max_bytes = static_cast<size_t>(std::stoull(std::string(env))); } catch (...) {}
    }

    std::string result;
    result.reserve(max_bytes);
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return result;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        if (result.size() < max_bytes) {
            const size_t chunk = std::strlen(buffer);
            const size_t remaining = max_bytes - result.size();
            result.append(buffer, (chunk < remaining) ? chunk : remaining);
        } else {
            // Drain remaining output to avoid blocking the child process,
            // but do not append to the result to cap memory growth.
            continue;
        }
    }
    pclose(pipe);
    return result;
}

} // namespace e2e_helpers
