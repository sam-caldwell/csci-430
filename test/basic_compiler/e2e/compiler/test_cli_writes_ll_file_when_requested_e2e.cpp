// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

static std::string cliPath_ll() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.WritesLLFileWhenRequested
 * Purpose: Validate --ll writes an LLVM IR file and returns success.
 * Components Under Test: CLI IR emission, filesystem.
 * Expected Behavior: Command exits 0, IR file exists and contains a main definition.
 */
TEST(CLI, WritesLLFileWhenRequested) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_ll";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "a.bas";
    const fs::path ll  = tmp / "a.ll";
    { std::ofstream f(bas); f << "10 PRINT 5\n"; }
    const std::string cmd = '"' + cliPath_ll() + '"' + " \"" + bas.string() + "\" --ll \"" + ll.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
    ASSERT_TRUE(fs::exists(ll));
    // Verify IR content
    std::ifstream in(ll); std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    ASSERT_NE(s.find("define i32 @main()"), std::string::npos);
}
