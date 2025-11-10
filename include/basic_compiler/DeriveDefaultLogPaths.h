// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_DERIVEDEFAULTLOGPATHS_H
#define BASIC_COMPILER_DERIVEDEFAULTLOGPATHS_H

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
                           bool noLogs,
                           std::optional<std::string> &logPath,
                           std::optional<std::string> &lexLogPath,
                           std::optional<std::string> &syntaxLogPath,
                           std::optional<std::string> &semanticLogPath);

#endif // BASIC_COMPILER_DERIVEDEFAULTLOGPATHS_H
