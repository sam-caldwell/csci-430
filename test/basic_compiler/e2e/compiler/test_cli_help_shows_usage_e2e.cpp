// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

static std::string cliPath_help() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.HelpShowsUsage
 * Purpose: Confirm --help prints usage text and exits 0.
 * Components Under Test: CLI help path.
 * Expected Behavior: Output contains "Usage:" and EXIT:0.
 */
TEST(CLI, HelpShowsUsage) {
    namespace fs = std::filesystem;
    const std::string cmd = '"' + cliPath_help() + '"' + " --help 2>&1; echo EXIT:$?";
    const const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("Usage:"), std::string::npos);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
}
