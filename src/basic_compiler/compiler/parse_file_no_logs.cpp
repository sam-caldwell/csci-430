// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include <fstream>
#include "basic_compiler/compiler/FileOpenError.h"

namespace gwbasic::phase_log_helpers {

/*
 * Function: parseFileNoLogs
 * Purpose:
 *  - Read and parse a GW-BASIC source file without enabling logging.
 * Inputs:
 *  - fpath: Filesystem path to source file
 * Outputs:
 *  - Program: Parsed AST
 * Throws:
 *  - gwbasic::FileOpenError when the file cannot be opened.
 */
gwbasic::Program parseFileNoLogs(const std::string& fpath) {
    std::ifstream fin(fpath);
    if (!fin) throw gwbasic::FileOpenError(fpath);
    Lexer lx(fin);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    return p.parseProgram();
}

} // namespace gwbasic::phase_log_helpers
