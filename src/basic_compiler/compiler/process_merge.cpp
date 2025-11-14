// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/Program.h"

#include <string>
#include <utility>

namespace gwbasic::phase_log_helpers {

/*
 * Function: processMerge
 * Purpose:
 *  - Handle MERGE directive logic by parsing the included program and
 *    appending its lines into 'program', replacing duplicate numbers.
 * Inputs:
 *  - incPath: Include path (canonicalized internally)
 *  - program: Destination program to mutate
 */
void processMerge(const std::string& incPath, gwbasic::Program& program) {
    const std::string canon = canonicalPath(incPath);
    auto mprog = parseFileNoLogs(canon);
    appendMergeProgramReplacing(program, std::move(mprog));
}

} // namespace gwbasic::phase_log_helpers
