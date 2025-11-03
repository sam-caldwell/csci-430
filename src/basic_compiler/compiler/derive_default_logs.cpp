// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/DeriveDefaultLogPaths.h"
#include <filesystem>

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
                           bool noLogs,
                           std::optional<std::string> &logPath,
                           std::optional<std::string> &lexLogPath,
                           std::optional<std::string> &syntaxLogPath,
                           std::optional<std::string> &semanticLogPath) {
    if (noLogs) return;
    if (!logPath) {
        std::filesystem::path p = input; p.replace_extension(".codegen.log");
        logPath = p.string();
    }
    if (!lexLogPath) {
        std::filesystem::path p = input; p.replace_extension(".lex.log");
        lexLogPath = p.string();
    }
    if (!syntaxLogPath) {
        std::filesystem::path p = input; p.replace_extension(".syntax.log");
        syntaxLogPath = p.string();
    }
    if (!semanticLogPath) {
        std::filesystem::path p = input; p.replace_extension(".semantic.log");
        semanticLogPath = p.string();
    }
}
