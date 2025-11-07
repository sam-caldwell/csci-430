// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "basic_compiler/compiler/PhaseLogHelpers.h"

using namespace gwbasic::phase_log_helpers;

/***
 * Test: PhaseLogHelpers.TokenizeRootWithLogs
 * Purpose: Ensure tokenizeRootWithLogs tokenizes a root file and reports the minimum line number.
 * Components Under Test: phase_log_helpers::tokenizeRootWithLogs().
 * Expected Behavior: Program is non-empty and minLine reflects the first line number.
 */
TEST(PhaseLogHelpers, TokenizeRootWithLogs) {
    auto d = std::filesystem::temp_directory_path() / "gwb_phase_root";
    std::filesystem::create_directories(d);
    auto f = d / "root.bas"; std::ofstream(f.string()) << "10 END\n";
    auto lex = (d/"lex.log").string(); auto syn=(d/"syn.log").string();
    std::string canon; int minLine=0; auto prog = tokenizeRootWithLogs(f.string(), lex, syn, canon, minLine);
    ASSERT_FALSE(prog.lines.empty());
    ASSERT_EQ(minLine, 10);
}
