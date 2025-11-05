// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include <filesystem>

namespace gwbasic::phase_log_helpers {

/*
 * Function: resolvePath
 * Purpose:
 *  - Resolve a possibly relative path against a base file path to a
 *    weakly canonical absolute path string.
 * Inputs:
 *  - baseFile: Full path of the file performing the include.
 *  - rel: Relative or absolute include path to resolve.
 * Outputs:
 *  - std::string: Resolved absolute path (weakly canonical).
 */
std::string resolvePath(const std::string& baseFile, const std::string& rel) {
    const std::filesystem::path p(rel);
    if (p.is_absolute()) return canonicalPath(p.string());
    const std::filesystem::path b(baseFile);
    return canonicalPath((b.parent_path() / p).string());
}

} // namespace gwbasic::phase_log_helpers

