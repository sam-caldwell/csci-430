// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <optional>
#include <string>

/**
 * Function: deriveDefaultLogPaths
 * Purpose:
 *  - Populate default phase-log file paths derived from the input file name.
 * Inputs:
 *  - input: Source filename (.bas) used to derive log filenames.
 *  - noLogs: If true, leaves paths untouched.
 *  - logPath, lexLogPath, syntaxLogPath, semanticLogPath: Optional outputs to fill.
 * Outputs:
 *  - void (updates optionals in place when not already set).
 */
void deriveDefaultLogPaths(const std::string &input,
                           const bool noLogs,
                           std::optional<std::string> &logPath,
                           std::optional<std::string> &lexLogPath,
                           std::optional<std::string> &syntaxLogPath,
                           std::optional<std::string> &semanticLogPath);
