// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

static std::string cliPath_stdout() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.StdoutIRWhenNoOutputsRequested
 * Purpose: Verify that without output flags the compiler prints IR to stdout and exits 0.
 * Components Under Test: CLI default output path, IR generation.
 * Expected Behavior: Output contains a main definition and EXIT:0.
 */
TEST(CLI, StdoutIRWhenNoOutputsRequested) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_stdout";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "c.bas";
    { std::ofstream f(bas); f << "10 PRINT 5\n"; }
    const std::string cmd = '"' + cliPath_stdout() + '"' + " \"" + bas.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("define i32 @main()"), std::string::npos);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
}
