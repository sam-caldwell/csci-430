// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

static std::string cliPath_e2e_ua() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.UnknownArgumentYieldsUsageExit2
 * Purpose: Check that an unknown CLI argument prints usage and exits with code 2.
 * Components Under Test: CLI argument parsing and error handling.
 * Expected Behavior: Output contains "Unknown argument:" and "Usage:", and exit code marker is EXIT:2.
 */
TEST(CLI, UnknownArgumentYieldsUsageExit2) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_unknown";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "prog.bas";
    { std::ofstream f(bas); f << "10 PRINT 42\n"; }
    const std::string cmd = '"' + cliPath_e2e_ua() + '"' + " \"" + bas.string() + "\" --bogus 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("Unknown argument:"), std::string::npos);
    ASSERT_NE(out.find("Usage:"), std::string::npos);
    ASSERT_NE(out.find("EXIT:2"), std::string::npos);
}
