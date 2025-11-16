// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

static std::string cliPath_e2e_badtriple() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.InvalidTargetTripleExits2
 * Purpose: Verify that an invalid --target triple is rejected with exit code 2 and an error message.
 * Components Under Test: CLI target triple validation.
 * Expected Behavior: Output mentions unsupported target triple and EXIT:2.
 */
TEST(CLI, InvalidTargetTripleExits2) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_target";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "prog.bas";
    { std::ofstream f(bas); f << "10 PRINT 1\n"; }
    const std::string cmd = '"' + cliPath_e2e_badtriple() + '"' + " \"" + bas.string() + "\" --target not_a_real_triple 2>&1; echo EXIT:$?";
    const const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("unsupported target triple"), std::string::npos);
    ASSERT_NE(out.find("EXIT:2"), std::string::npos);
}
