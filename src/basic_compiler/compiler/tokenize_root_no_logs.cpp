// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"

namespace gwbasic::phase_log_helpers {

/*
 * Function: tokenizeRootNoLogs
 * Purpose:
 *  - Parse the root source without logs, returning its AST along with
 *    canonical path and minimum line number for renumbering context.
 * Inputs:
 *  - path: Filesystem path to root source
 * Outputs:
 *  - Program: Parsed AST
 *  - outCanon: Canonical path to the root file
 *  - outMinLine: Minimum line number or 0 if program is empty
 */
gwbasic::Program tokenizeRootNoLogs(const std::string& path,
                                    std::string& outCanon,
                                    int& outMinLine) {
    auto prog = parseFileNoLogs(path);
    outCanon = canonicalPath(path);
    int minRoot = INT_MAX;
    for (const auto&[number, statements] : prog.lines)
        if (number < minRoot) minRoot = number;
    outMinLine = (minRoot == INT_MAX ? 0 : minRoot);
    return prog;
}

} // namespace gwbasic::phase_log_helpers

