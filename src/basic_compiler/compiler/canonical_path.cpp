// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include <filesystem>

namespace gwbasic::phase_log_helpers {

/*
 * Function: canonicalPath
 * Purpose:
 *  - Produce a weakly canonical absolute path string for stable map keys.
 * Inputs:
 *  - p: Input path (relative or absolute)
 * Outputs:
 *  - std::string: Weakly canonical absolute path.
 */
std::string canonicalPath(const std::string& p) {
    return std::filesystem::weakly_canonical(std::filesystem::path(p)).string();
}

} // namespace gwbasic::phase_log_helpers

