// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/lexer/Lexer.h"
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/Program.h"
#include <fstream>
#include <string>
#include <utility>
#include <climits>
#include "basic_compiler/compiler/FileOpenError.h"

namespace gwbasic::phase_log_helpers {

/*
 * Function: tokenizeRootWithLogs
 * Purpose:
 *  - Tokenize and parse the root source file with lexer and syntax logs.
 * Inputs:
 *  - path: Source path
 *  - lexLogPath/syntaxLogPath: Paths for phase logs
 * Outputs:
 *  - Program: Parsed root program
 *  - outCanon: Canonical path to the root file
 *  - outMinLine: Minimum line in the root program (or 0 when empty)
 * Throws:
 *  - gwbasic::FileOpenError when the root file cannot be opened.
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
gwbasic::Program tokenizeRootWithLogs(const std::string& path,
                                      const std::string& lexLogPath,
                                      const std::string& syntaxLogPath,
                                      std::string& outCanon,
                                      int& outMinLine) {
    std::ifstream input(path);
    if (!input) {
        throw gwbasic::FileOpenError(path);
    }
    Lexer lex(input);
    lex.setLexLogPath(lexLogPath);
    auto toks = lex.tokenize();
    Parser parser(std::move(toks));
    parser.setSyntaxLogPath(syntaxLogPath);
    auto prog = parser.parseProgram();
    outCanon = canonicalPath(path);
    int minRoot = INT_MAX;
    for (const auto& [number, statements] : prog.lines) {
        if (number < minRoot) {
            minRoot = number;
        }
    }
    outMinLine = (minRoot == INT_MAX ? 0 : minRoot);
    return prog;
}

} // namespace gwbasic::phase_log_helpers
