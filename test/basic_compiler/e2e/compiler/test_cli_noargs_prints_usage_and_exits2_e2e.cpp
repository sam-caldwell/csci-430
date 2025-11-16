// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

static std::string cliPath_noargs() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.NoArgsPrintsUsageAndExits2
 * Purpose: Ensure invoking CLI with no arguments prints usage and exits with code 2.
 * Components Under Test: CLI argument validation.
 * Expected Behavior: Output contains "Usage:" and EXIT:2.
 */
TEST(CLI, NoArgsPrintsUsageAndExits2) {
    const std::string cmd = '"' + cliPath_noargs() + '"' + " 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("Usage:"), std::string::npos);
    ASSERT_NE(out.find("EXIT:2"), std::string::npos);
}
