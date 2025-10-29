// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <sstream>
#include <string>

#include "run_command.h"

namespace e2e_helpers {

/**
 * Function: toolExists
 * Purpose:
 *  - Check if a given tool is available on PATH.
 * Inputs:
 *  - tool: Executable name (e.g., "clang").
 * Outputs:
 *  - bool: true if the tool resolves via `command -v`.
 */
inline bool toolExists(const std::string& tool) {
    std::ostringstream oss; oss << "sh -c 'command -v \"" << tool << "\"'";
    const std::string out = runCommand(oss.str());
    return !out.empty();
}

} // namespace e2e_helpers

