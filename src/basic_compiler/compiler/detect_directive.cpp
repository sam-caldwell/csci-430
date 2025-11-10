// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/MergeStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/RunStmt.h"
#include <string>

namespace gwbasic::phase_log_helpers {

/*
 * Function: detectDirective
 * Purpose:
 *  - Inspect a line for MERGE/CHAIN/RUN directives and compute the include path.
 * Inputs:
 *  - ln: Line to inspect
 *  - curPath: Canonical path to the current source file
 *  - dir/outIncPath: Outputs for directive kind and resolved include path
 * Outputs:
 *  - bool: true if a directive was found; false otherwise
 */
// NOLINTNEXTLINE(readability-function-size)
// NOLINTNEXTLINE(readability-function-size)
bool detectDirective(const gwbasic::Line& lineObj,
                     const std::string& curPath,
                     Dir& dir,
                     std::string& outIncPath) {

    using enum gwbasic::phase_log_helpers::Dir;
    dir = None; outIncPath.clear();

    for (const auto& stmtPtr : lineObj.statements) {
        if (const auto* mergeStmt = gwbasic::dyn_cast<gwbasic::MergeStmt>(stmtPtr.get())) {
            dir = Merge;
            outIncPath = resolvePath(curPath, mergeStmt->filename);
            return true;
        }
        if (const auto* chainStmt = gwbasic::dyn_cast<gwbasic::ChainStmt>(stmtPtr.get())) {
            if (chainStmt->filename.has_value()) {
                dir = Chain;
                outIncPath = resolvePath(curPath, *chainStmt->filename);
                return true;
            }
        }
        if (const auto* runStmt = gwbasic::dyn_cast<gwbasic::RunStmt>(stmtPtr.get())) {
            if (runStmt->filename.has_value()) {
                dir = Run;
                outIncPath = resolvePath(curPath, *runStmt->filename);
                return true;
            }
        }
    }
    return false;

}

} // namespace gwbasic::phase_log_helpers
