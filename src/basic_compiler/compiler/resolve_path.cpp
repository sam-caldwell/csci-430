// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include <filesystem>
#include <string>

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
    const std::filesystem::path relPath(rel);
    if (relPath.is_absolute()) { return canonicalPath(relPath.string()); }
    const std::filesystem::path basePath(baseFile);
    return canonicalPath((basePath.parent_path() / relPath).string());
}

} // namespace gwbasic::phase_log_helpers
