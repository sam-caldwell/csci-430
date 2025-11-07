// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "basic_compiler/compiler/PhaseLogHelpers.h"

using namespace gwbasic::phase_log_helpers;

static std::filesystem::path mktempdir_plh_parse(const char* name) {
    auto d = std::filesystem::temp_directory_path() / name;
    std::filesystem::create_directories(d);
    return d;
}

/***
 * Test: PhaseLogHelpers.ParseFileNoLogs_HappySad
 * Purpose: Validate parseFileNoLogs loads an existing file and throws for a missing file.
 * Components Under Test: phase_log_helpers::parseFileNoLogs().
 * Expected Behavior: Non-empty program for valid path; std::runtime_error for missing path.
 */
TEST(PhaseLogHelpers, ParseFileNoLogs_HappySad) {
    auto d = mktempdir_plh_parse("gwb_phase_parse");
    auto f = d / "one.bas";
    std::ofstream ofs(f.string()); ofs << "10 PRINT 1\n20 END\n"; ofs.close();
    auto [lines] = parseFileNoLogs(f.string());
    ASSERT_FALSE(lines.empty());
    // Sad path: missing file
    ASSERT_THROW(parseFileNoLogs((d/"missing.bas").string()), std::runtime_error);
}
