// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include <fstream>
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
    std::ifstream in(path);
    if (!in) throw gwbasic::FileOpenError(path);
    Lexer lex(in);
    lex.setLexLogPath(lexLogPath);
    auto toks = lex.tokenize();
    Parser rp(std::move(toks));
    rp.setSyntaxLogPath(syntaxLogPath);
    auto prog = rp.parseProgram();
    outCanon = canonicalPath(path);
    int minRoot = INT_MAX;
    for (const auto&[number, statements] : prog.lines)
        if (number < minRoot) minRoot = number;
    outMinLine = (minRoot == INT_MAX ? 0 : minRoot);
    return prog;
}

} // namespace gwbasic::phase_log_helpers
