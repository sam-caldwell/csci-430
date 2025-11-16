// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

static std::string cliPath_e2e_printtriple() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.PrintTriplePrintsAndExits0
 * Purpose: Confirm --print-triple prints the default target triple and exits 0 without needing an input file.
 * Components Under Test: CLI print-triple path.
 * Expected Behavior: Output contains a hyphenated triple and EXIT:0.
 */
TEST(CLI, PrintTriplePrintsAndExits0) {
    // CLI expects an input path; print-triple exits before reading it
    const std::string cmd = '"' + cliPath_e2e_printtriple() + '"' + " dummy.bas --print-triple 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    // Expect something like x86_64-apple-darwin or arm64-apple-macosx...
    ASSERT_NE(out.find("-"), std::string::npos);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
}
