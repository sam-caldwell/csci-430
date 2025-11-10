// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/util/TransparentSVHasher.h"
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace gwbasic::phase_log_helpers {

/*
 * Function: processChainRun
 * Purpose:
 *  - Handle CHAIN/RUN directive logic: ensure import, then patch the
 *    target line number on the directive to base+first-line for the
 *    imported program (or respect explicit target if present).
 * Inputs:
 *  - curPath: Canonical path of current file
 *  - incPath: Include path (canonicalized internally)
 *  - isChain: true for CHAIN; false for RUN
 *  - imported: Map (path -> {base, minLine}) for imported programs
 *  - ln: Line containing the directive; patched in-place
 * Outputs:
 *  - out: Populated when a new import is parsed and renumbered
 *  - bool: true if a new import occurred; false otherwise
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
bool processChainRun(const std::string& curPath,
                     const std::string& incPath,
                     const bool isChain,
                     std::unordered_map<std::string, std::pair<int,int>,
                     TransparentSVHasher, std::equal_to<>>& imported,
                     Line& lineObj,
                     ImportedProg& out) {
    const std::string canon = canonicalPath(incPath);
    const bool added = ensureImported(canon, curPath, imported, out);
    const auto itImported = imported.find(std::string_view(canon));
    patchTargetsForChainOrRun(lineObj, itImported->second, isChain);
    return added;
}

} // namespace gwbasic::phase_log_helpers
