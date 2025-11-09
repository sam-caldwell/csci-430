// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/DeriveDefaultLogPaths.h"
#include <optional>
#include <string>

// NOLINTBEGIN(llvmlibc-implementation-in-namespace)

/*
 * Function: deriveDefaultLogPaths
 * Inputs:
 *  - input: Source file path used to derive log names
 *  - noLogs: When true, leaves outputs unchanged
 *  - logPath, lexLogPath, syntaxLogPath, semanticLogPath: optionals to fill
 * Outputs:
 *  - void (sets optionals when not already provided by caller)
 * Theory of operation:
 *  - Replaces the input extension with '.codegen.log', '.lex.log',
 *    '.syntax.log', and '.semantic.log' as needed.
 */
void deriveDefaultLogPaths(const std::string &input,
                           const bool noLogs,
                           std::optional<std::string> &logPath,
                           std::optional<std::string> &lexLogPath,
                           std::optional<std::string> &syntaxLogPath,
                           std::optional<std::string> &semanticLogPath) {
    if (noLogs) {
        return;
    }

    auto replace_ext = [](const std::string &path, const std::string &new_ext) -> std::string {
        const auto slash = path.find_last_of("/\\");
        const auto dot = path.find_last_of('.');
        const bool has_ext = (dot != std::string::npos) && (slash == std::string::npos || dot > slash);
        if (has_ext) {
            return path.substr(0, dot) + new_ext;
        }
        return path + new_ext;
    };

    if (!logPath) {
        logPath = replace_ext(input, ".codegen.log");
    }
    if (!lexLogPath) {
        lexLogPath = replace_ext(input, ".lex.log");
    }
    if (!syntaxLogPath) {
        syntaxLogPath = replace_ext(input, ".syntax.log");
    }
    if (!semanticLogPath) {
        semanticLogPath = replace_ext(input, ".semantic.log");
    }
}

// NOLINTEND(llvmlibc-implementation-in-namespace)
