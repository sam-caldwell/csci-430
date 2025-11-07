// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <sstream>
#include <string>

#include "run_command.h"

namespace e2e_helpers {

/**
 * Function: runCommandWithInput
 * Purpose:
 *  - Run an executable and feed it stdin, capturing stdout.
 * Inputs:
 *  - exe: Path to executable to run.
 *  - input: Bytes to send to stdin.
 * Outputs:
 *  - std::string: Captured stdout from the process.
 */
inline std::string runCommandWithInput(const std::string& exe, const std::string& input) {
    std::ostringstream c; c << "sh -c 'printf \"" << input << "\" | \"" << exe << "\"'";
    return runCommand(c.str());
}

} // namespace e2e_helpers

