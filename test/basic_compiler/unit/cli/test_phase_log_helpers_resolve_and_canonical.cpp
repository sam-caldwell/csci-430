// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "basic_compiler/compiler/PhaseLogHelpers.h"

using namespace gwbasic::phase_log_helpers;

static std::filesystem::path mktempdir_plh(const char* name) {
    auto d = std::filesystem::temp_directory_path() / name;
    std::filesystem::create_directories(d);
    return d;
}

/***
 * Test: PhaseLogHelpers.ResolveAndCanonical
 * Purpose: Verify resolvePath returns the canonical path of a child file relative to a base file.
 * Components Under Test: phase_log_helpers::resolvePath(), canonicalPath().
 * Expected Behavior: Resolved path equals canonical filesystem path for the child.
 */
TEST(PhaseLogHelpers, ResolveAndCanonical) {
    const auto d = mktempdir_plh("gwb_phase_paths");
    const auto parent = d / "p"; std::filesystem::create_directories(parent);
    const auto baseFile = parent / "file.bas"; std::ofstream(baseFile.string()).put('\n');
    const auto child = parent / "child.bas"; std::ofstream(child.string()).put('\n');
    const auto out = resolvePath(baseFile.string(), "child.bas");
    ASSERT_EQ(out, canonicalPath(child.string()));
}
