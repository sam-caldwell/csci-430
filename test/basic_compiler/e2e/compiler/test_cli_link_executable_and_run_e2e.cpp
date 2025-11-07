// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/clang_path.h"

using namespace e2e_helpers;

static std::string cliBin_link() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.LinkExecutableAndRun
 * Purpose: Ensure CLI links an executable (-o) and the resulting binary runs and prints expected output.
 * Components Under Test: CLI driver (linking), Clang toolchain, process execution/output capture.
 * Expected Behavior: Command exits 0, executable exists, and running it prints the expected line.
 */
TEST(CLI, LinkExecutableAndRun) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP() << "clang not found; skipping";
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_link";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "p.bas";
    const fs::path exe = tmp / "prog.out";
    { std::ofstream f(bas); f << "10 PRINT 9\n20 END\n"; }
    const std::string cmd = '"' + cliBin_link() + '"' + " \"" + bas.string() + "\" -o \"" + exe.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos) << out;
    ASSERT_TRUE(fs::exists(exe));
    const std::string runOut = runCommand('"' + exe.string() + '"');
    ASSERT_NE(runOut.find("9\n"), std::string::npos);
}
